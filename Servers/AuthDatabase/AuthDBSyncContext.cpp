/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

/*!
 * @addtogroup AuthDatabase Projects/CoX/Servers/AuthDatabase
 * @{
 */

#include "AuthDBSyncContext.h"

#include "Messages/AuthDatabase/AuthDBSyncEvents.h"
#include "Components/PasswordHasher.h"
#include "Components/Settings.h"
#include "Version.h"

#include <ace/Thread.h>

#include <QDateTime>
#include <QDebug>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <cassert>

using namespace SEGSEvents;

namespace
{
    static const auto DATABASE_DRIVERS = QStringList {"QSQLITE", "QPSQL", "QMYSQL"};

    static constexpr int64_t INVALID_DB_VERSION = -1;

    static constexpr auto FETCH_DB_VERSION_QUERY =
        "SELECT version FROM table_versions WHERE table_name = 'accounts' LIMIT 1";

    static constexpr auto ADD_ACCOUNT_QUERY =
        "INSERT INTO accounts (username, passw, access_level, salt) VALUES (?, ?, ?, ?);";

    static constexpr auto SELECT_ACCOUNT_BY_USERNAME_QUERY =
        "SELECT * FROM accounts WHERE username = ?;";

    static constexpr auto SELECT_ACCOUNT_BY_ID_QUERY =
        "SELECT * FROM accounts WHERE id = ?;";

    static constexpr auto SELECT_ACCOUNT_PASSWORD_QUERY =
        "SELECT passw, salt FROM accounts WHERE username = ?;";
    static constexpr std::pair<const char *,AuthDbSyncContext::QueryId> all_query_texts[] = {
        {FETCH_DB_VERSION_QUERY,AuthDbSyncContext::ID_FETCH_DB_VERSION_QUERY},
        {ADD_ACCOUNT_QUERY,AuthDbSyncContext::ID_ADD_ACCOUNT_QUERY},
        {SELECT_ACCOUNT_BY_USERNAME_QUERY,AuthDbSyncContext::ID_SELECT_ACCOUNT_BY_USERNAME_QUERY},
        {SELECT_ACCOUNT_BY_ID_QUERY,AuthDbSyncContext::ID_SELECT_ACCOUNT_BY_ID_QUERY},
        {SELECT_ACCOUNT_PASSWORD_QUERY,AuthDbSyncContext::ID_SELECT_ACCOUNT_PASSWORD_QUERY},
    };
    //! @image html dbschema/segs_dbschema.png
    bool GetAccount(RetrieveAccountResponseData &client, QSqlQuery &results)
    {
        if(!results.next()) // TODO: handle case of multiple accounts with same name ?
            return false;
        QDateTime creation;
        client.m_acc_server_acc_id = results.value("id").toULongLong();
        client.m_login             = qPrintable(results.value("username").toString());
        client.m_access_level      = results.value("access_level").toUInt();
        creation                   = results.value("creation_date").toDateTime();
        //  client->setCreationDate(creation);
        return true;
    }

    int getDatabaseVersion(QSqlDatabase &database)
    {
        QSqlQuery query(FETCH_DB_VERSION_QUERY, database);

        if(!query.exec())
        {
            qDebug() << query.lastError();
            return INVALID_DB_VERSION;
        }

        if(!query.next())
        {
            qCritical() << "No rows found when fetching database version.";
            return INVALID_DB_VERSION;
        }

        return query.value(0).toLongLong();
    }


} // namespace

AuthDbSyncContext::AuthDbSyncContext() {}

AuthDbSyncContext::~AuthDbSyncContext() {}

// Maybe one day we'll need to read different db configs per-thread, for now all just read the same file.
bool AuthDbSyncContext::loadAndConfigure()
{
    ACE_Thread_ID our_id;

    if(m_setup_complete)
    {
        qCritical() << "This AuthDBSyncContext has already been configured!";
        return false;
    }

    qInfo() << "Loading AuthDbSync settings...";
    Settings config(Settings::getSettingsPath());

    config.beginGroup(("AdminServer"));
    config.beginGroup(("AccountDatabase"));
    String dbdriver = config.value<String>(("db_driver"), "QSQLITE");
    String dbhost   = config.value<String>(("db_host"), "127.0.0.1");
    int    dbport   = config.value<int>(("db_port"), 5432);
    String dbname   = config.value<String>(("db_name"), "segs.db");
    String dbuser   = config.value<String>(("db_user"), "segsadmin");
    String dbpass   = config.value<String>(("db_pass"), "segs123");
    config.endGroup(); // AccountDatabase
    config.endGroup(); // AdminServer

    if(!DATABASE_DRIVERS.contains(dbdriver.to_upper().c_str()))
    {
        sWarning() << "Database driver" << dbdriver << "is not supported.";
        return false;
    }

    char thread_name_buf[16];
    our_id.to_string(thread_name_buf); // Ace is using template specialization to acquire the length of passed buffer

    QSqlDatabase *db2 =
        new QSqlDatabase(QSqlDatabase::addDatabase(dbdriver.c_str(), QStringLiteral("AdminDatabase_") + thread_name_buf));
    db2->setHostName(dbhost.c_str());
    db2->setPort(dbport);
    db2->setDatabaseName(dbname.c_str());
    db2->setUserName(dbuser.c_str());
    db2->setPassword(dbpass.c_str());
    m_db.reset(db2); // at this point we become owner of the db

    if(dbdriver == "QMYSQL")
    {
        db2->setConnectOptions("MYSQL_OPT_RECONNECT=true");
    }

    if(!m_db->open())
    {
        sCritical() << "Failed to open database:" << dbname;
        db2->setConnectOptions();
        return false;
    }

    int db_version = getDatabaseVersion(*m_db);
    int required_db_version = VersionInfo::getRequiredAuthDBVersion();

    if(db_version != required_db_version)
    {
        // we should just stop the server, it isn't going to work anyway
        qFatal("Wrong database version (%d) Auth database requires version: %d", db_version, required_db_version);
        db2->setConnectOptions();
        return false;
    }
    for(const std::pair<const char *,QueryId> &v : all_query_texts)
    {
        m_query_mapping[v.second].reset(new QSqlQuery(*m_db));
        if(!m_query_mapping[v.second]->prepare(v.first))
        {
            qDebug() << "SQL_ERROR:" << m_query_mapping[v.second]->lastError();
            return false;
        }
    }
    return true;
}

bool AuthDbSyncContext::addAccount(const CreateAccountData &data)
{
    PasswordHasher hasher;
    String     salt            = hasher.generateSalt();
    auto         hashed_password = hasher.hashPassword(data.password, salt);
    const auto &qr(m_query_mapping[ID_ADD_ACCOUNT_QUERY]);
    qr->bindValue(0, data.username.c_str());
    qr->bindValue(1, (const char *)hashed_password.data());
    qr->bindValue(2, data.access_level);
    qr->bindValue(3, salt.c_str());
    if(false == qr->exec()) // Send our query to the PostgreSQL db server to process
    {
        last_error.reset(new QSqlError(qr->lastError()));
        qDebug() << "SQL_ERROR:" << *last_error; // Why the query failed
        return false;
    }
    return true;
}

// bool AuthDbSyncContext::retrieveAccount(const RetrieveAccountRequestData &data,RetrieveAccountResponseData &result)
//{
//    result.m_acc_server_acc_id = 0;
//    if(data.m_id==0)
//    {
//        m_prepared_select_account_by_username->bindValue(0,data.m_login);
//        if(!m_prepared_select_account_by_username->exec()) {
//            last_error.reset(new QSqlError(m_prepared_select_account_by_username->lastError()));
//            qDebug() << "SQL_ERROR:"<< *last_error; // Why the query failed
//            return false;
//        }
//        return GetAccount(result,*m_prepared_select_account_by_username);
//    }
//    else
//    {
//        m_prepared_select_account_by_id->bindValue(0,data.m_id);
//        if(!m_prepared_select_account_by_id->exec()) {
//            last_error.reset(new QSqlError(m_prepared_select_account_by_id->lastError()));
//            qDebug() << "SQL_ERROR:"<< *last_error; // Why the query failed
//            return false;
//        }
//        return GetAccount(result,*m_prepared_select_account_by_username);
//    }
//}

bool AuthDbSyncContext::checkPassword(const String &login, const String &password)
{
    const auto &qr(m_query_mapping[ID_SELECT_ACCOUNT_PASSWORD_QUERY]);

    qr->bindValue(0, login.c_str());

    if(!qr->exec())
    {
        last_error.reset(new QSqlError(qr->lastError()));
        qDebug() << "SQL_ERROR:" << *last_error; // Why the query failed
        return false;
    }
    if(!qr->next())
    {
        return false;
    }
    PasswordHasher hasher;
    String     required_pass = qr->value("passw").toByteArray().data();
    String     salt          = qr->value("salt").toByteArray().data();
    // TODO: remove
    auto hashed_password = hasher.hashPassword(password.data(), salt);
    return required_pass.size() == hashed_password.size() &&
           memcmp(hashed_password.data(), required_pass.data(), required_pass.size()) == 0;
}

bool AuthDbSyncContext::retrieveAccountAndCheckPassword(
    const RetrieveAccountRequestData &request, RetrieveAccountResponseData &response)
{
    response.m_acc_server_acc_id = RetrieveAccountResponseData::INVALID_ACCOUNT_ID;
    assert(request.m_id == 0);

    if(!checkPassword(request.m_login, request.m_password))
    {
        if(!last_error)
        {
            response.m_acc_server_acc_id = RetrieveAccountResponseData::INVALID_ACCOUNT_ID;
            return true;
        }

        return !last_error->isValid();
    }
    const auto &qr(m_query_mapping[ID_SELECT_ACCOUNT_BY_USERNAME_QUERY]);

    qr->bindValue(0, request.m_login.c_str());

    if(!qr->exec())
    {
        last_error.reset(new QSqlError(qr->lastError()));
        qDebug() << "SQL_ERROR:" << *last_error; // Why the query failed
        return false;
    }

    return GetAccount(response, *qr);
}

bool AuthDbSyncContext::getPasswordValidity(const ValidatePasswordRequestData &data,
                                            ValidatePasswordResponseData &     result)
{
    result.m_valid_password = checkPassword(data.username, data.password);
    return !last_error->isValid();
}

//! @}
