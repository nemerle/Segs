/*
 * SEGS - Super Entity Game Server - dbtool
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2018 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#include "DBMigrationStep.h"
#include "Components/Logging.h"

#include <QtSql/QtSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

DBMigrationStep::DBMigrationStep()
{
}

bool DBMigrationStep::canRun(DBConnection *db, int cur_version)
{
    // migrations_to_run may contain both databases, skip
    // databases that don't match the one we're currently checking
    if(getName() != db->getName())
    {
        sCFDebug(logMigration,"We're currently looking for %s database, but found %s. Skipping to the next migration in the list.",
                          qPrintable(getName()), qPrintable(db->getName()));
        return false;
    }

    // skip migrations with a target version beneath the current db version
    if(getTargetVersion() <= cur_version)
    {
        sCFDebug(logMigration,"Migration step version %d is beneath current database version %d. Skipping to the next one.",
                          getTargetVersion(),cur_version);
        return false;
    }

    // if current database version is one less than the target version, run it.
    if(getTargetVersion() == cur_version + 1)
        return true;

    sCFDebug(logMigration,"Cannot run migration step %d on %s database.",getTargetVersion(),qPrintable(db->getName()));
    return false;
}

bool DBMigrationStep::cleanup(DBConnection *db)
{
    DBSchemas schemas = getTableVersions();
    // attempt to update table versions
    if(!db->updateTableVersions(schemas))
    {
        qWarning() << "Failed to update database schema versions! Rolling back database.";
        db->m_db->rollback();
        return false;
    }

    sCFDebug(logMigration,"Running commit on upgrade %d on %s...",getTargetVersion(),qPrintable(db->getName()));
    return true; // successful
}
