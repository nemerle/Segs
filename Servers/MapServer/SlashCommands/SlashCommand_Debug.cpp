/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

/*!
 * @addtogroup SlashCommands Projects/CoX/Servers/MapServer/SlashCommands
 * @{
 */

#include "SlashCommand_Debug.h"

#include "DataHelpers.h"
#include "GameData/Character.h"
#include "GameData/CharacterHelpers.h"
#include "GameData/EntityHelpers.h"
#include "Components/Logging.h"
#include "MapInstance.h"
#include "Messages/Map/Browser.h"
#include "Messages/Map/DoorMessage.h"
#include "Messages/Map/StandardDialogCmd.h"
#include "MessageHelpers.h"

#include <QtCore/QString>
#include <QtCore/QDebug>

using namespace SEGSEvents;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Access Level 9 Commands (GMs)
void cmdHandler_Dialog(const Vector<String> &params, MapClientSession &sess)
{
    sess.addCommandToSendNextUpdate(eastl::make_unique<StandardDialogCmd>(String::joined(params," ")));
}

void cmdHandler_InfoMessage(const Vector<String> &params, MapClientSession &sess)
{
    String msg;
    int cmdType = int(MessageChannel::USER_ERROR);

    if(params.size() < 2)
        msg = "The /imsg command takes two arguments, a <b>number</b> and a <b>string</b>";
    else
    {
        bool ok = true;
        cmdType = StringUtils::to_int(params[0],&ok);
        if(!ok || cmdType<1 || cmdType>21)
        {
            msg = "The first /imsg argument must be a <b>number</b> between 1 and 21";
            cmdType = int(MessageChannel::USER_ERROR);
        }
        else
            msg = params.at(1);
    }
    sendInfoMessage(static_cast<MessageChannel>(cmdType), msg, sess);
}

void cmdHandler_UpdateChar(const Vector<String> &/*params*/, MapClientSession &sess)
{
    markEntityForDbStore(sess.m_ent, DbStoreFlags::Full);

    String msg = "Updating Character in Database: " + sess.m_ent->name();
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

void cmdHandler_DebugChar(const Vector<String> &/*params*/, MapClientSession &sess)
{
    const Character &chardata(*sess.m_ent->m_char);
    String msg = "DebugChar: " + sess.m_ent->name()
            + "\n  " + chardata.m_char_data.m_origin_name
            + "\n  " + chardata.m_char_data.m_class_name
            + "\n  map: " + getEntityDisplayMapName(sess.m_ent->m_entity_data)
            + "\n  db_id: " + eastl::to_string(sess.m_ent->m_db_id) + ":" + eastl::to_string(chardata.m_db_id)
            + "\n  idx: " + eastl::to_string(sess.m_ent->m_idx)
            + "\n  access: " + eastl::to_string(sess.m_ent->m_entity_data.m_access_level)
            + "\n  acct: " + eastl::to_string(chardata.m_account_id)
            + "\n  lvl/clvl: " + eastl::to_string(chardata.m_char_data.m_level+1) + "/" + eastl::to_string(chardata.m_char_data.m_combat_level+1)
            + "\n  inf: " + eastl::to_string(chardata.m_char_data.m_influence)
            + "\n  xp/debt: " + eastl::to_string(chardata.m_char_data.m_experience_points) + "/" + eastl::to_string(chardata.m_char_data.m_experience_debt)
            + "\n  lfg: " + eastl::to_string(chardata.m_char_data.m_lfg)
            + "\n  afk: " + eastl::to_string(chardata.m_char_data.m_afk)
            + "\n  tgt_idx: " + eastl::to_string(getTargetIdx(*sess.m_ent));
    sess.m_ent->dump();
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

void cmdHandler_SendFloatingNumbers(const Vector<String> &params, MapClientSession &sess)
{
    Entity *tgt = nullptr;

    String msg; // result messages
    bool ok1 = true;
    bool ok2 = true;
    uint32_t runtimes  = StringUtils::to_int(params[0],&ok1);
    float    amount   = StringUtils::to_float(params[1],&ok2);
    String name        = params[2];

    // reign in the insanity
    runtimes = std::clamp(runtimes, uint32_t(0), uint32_t(5));

    if(!ok1 || !ok2 || name.empty())
    {
        msg = "FloatingNumbers takes three arguments: `/damage <number_times_to_run> <damage_amount> <target_name>`";
        sCDebug(logSlashCommand) << msg;
        sendInfoMessage(MessageChannel::USER_ERROR, msg, sess);
        return;
    }

    tgt = getEntity(&sess,name); // get Entity by name

    if(tgt == nullptr)
    {
        msg = "FloatingNumbers target " + name + " cannot be found.";
        sCDebug(logSlashCommand) << msg;
        sendInfoMessage(MessageChannel::USER_ERROR, msg, sess);
        return;
    }

    for(uint32_t i = 0; i<runtimes; i++)
    {
        sendFloatingNumbers(sess, tgt->m_idx, int(amount));

        setHP(*tgt->m_char, getHP(*tgt->m_char) - amount); // deal dmg

        if(amount >= 0) // damage
        {
            msg = String(String::CtorSprintf(),"%s deals %d points of damage to %s.",sess.m_ent->name().c_str(),amount,name.c_str());
            sCDebug(logSlashCommand) << msg;

            msg = String(String::CtorSprintf(), "You deal %d points of damage to %s.",amount, name.c_str());
            sendInfoMessage(MessageChannel::DAMAGE, msg, sess);
            msg = String(String::CtorSprintf(), "%s has dealt you %2 points of damage!", sess.m_ent->name().c_str(), amount);
            sendInfoMessage(MessageChannel::DAMAGE, msg, *tgt->m_client);
        }
        else
        {
            msg = String(String::CtorSprintf(), "%s heals %d points of damage from %s.",sess.m_ent->name().c_str(),-amount,name.c_str());
            sCDebug(logSlashCommand) << msg;

            msg = String(String::CtorSprintf(), "You heal %d points of damage from %s.",-amount,name.c_str());
            sendInfoMessage(MessageChannel::TEAM, msg, sess); // TEAM for green
            msg = String(String::CtorSprintf(), "%s has healed %d points of damage from you!",sess.m_ent->name().c_str(),-amount);
            sendInfoMessage(MessageChannel::TEAM, msg, *tgt->m_client); // TEAM for green
        }
    }
}

void cmdHandler_SetSequence(const Vector<String> &params, MapClientSession &sess)
{
    bool        update  = StringUtils::to_int(params.at(0));
    uint32_t    idx     = StringUtils::to_int(params.at(1));
    uint8_t     time    = StringUtils::to_int(params.at(2));

    String msg = "Setting Sequence " + eastl::to_string(idx) + " for " + eastl::to_string(time);
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);

    sess.m_ent->m_seq_update = update;
    sess.m_ent->m_seq_move_idx = idx;
    sess.m_ent->m_seq_move_change_time = time;
}

void cmdHandler_AddTriggeredMove(const Vector<String> &params, MapClientSession &sess)
{
    uint32_t move_idx, delay, fx_idx;
    move_idx    = StringUtils::to_int(params.at(0));
    delay       = StringUtils::to_int(params.at(1));
    fx_idx      = StringUtils::to_int(params.at(2));

    addTriggeredMove(*sess.m_ent, move_idx, delay, fx_idx);

    String msg = String(String::CtorSprintf(), "Setting TriggeredMove: idx %d;  ticks: %d;  fx_idx: %d",
                     move_idx,delay, fx_idx);
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

void cmdHandler_AddTimeStateLog(const Vector<String> &params, MapClientSession &sess)
{
    int val = StringUtils::to_int(params.at(0));

    if(val == 0)
        val = ::time(nullptr);

    sendTimeStateLog(sess, val);

    String msg = "Set TimeStateLog to: " + eastl::to_string(val);
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

void cmdHandler_SetClientState(const Vector<String> &params, MapClientSession &sess)
{
    int val = StringUtils::to_int(params.at(0));

    sendClientState(sess, ClientStates(val));

    String msg = "Setting ClientState to: " + eastl::to_string(val);
    //qCDebug(logSlashCommand) << msg; // we're already sending a debug msg elsewhere
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

void cmdHandler_LevelUpXp(const Vector<String> &params, MapClientSession &sess)
{
    // start with our current level + 1, unless provided with one
    uint32_t level = getLevel(*sess.m_ent->m_char) + 1;
    if(!params.empty())
        level = StringUtils::to_int(params.at(0));

    GameDataStore &data(getGameData());
    // must adjust level for 0-index array, capped at 49
    uint32_t max_level = data.expMaxLevel();
    level = std::max(uint32_t(0), std::min(level, max_level));

    // XP must be high enough for the level you're advancing to
    // since this slash command is forcing a levelup, let's
    // increase xp accordingly
    if(getXP(*sess.m_ent->m_char) < data.expForLevel(level))
        setXP(*sess.m_ent->m_char, data.expForLevel(level));
    else
        return;

    sCDebug(logPowers) << "LEVELUP" << sess.m_ent->name() << "to" << level + 1
                       << "NumPowers:" << countAllOwnedPowers(sess.m_ent->m_char->m_char_data, false) // no temps
                       << "NumPowersAtLevel:" << data.countForLevel(level, data.m_pi_schedule.m_Power);

    // send levelup pkt to client
    sess.m_ent->m_char->m_client_window_state = ClientWindowState::Training; // flag character so we can handle dialog response
    sendLevelUp(sess);
}

void cmdHandler_TestDeadNoGurney(const Vector<String> &/*params*/, MapClientSession &sess)
{
    sCDebug(logSlashCommand) << "Sending DeadNoGurney";
    sendDeadNoGurney(sess);
}

void cmdHandler_DoorMessage(const Vector<String> &params, MapClientSession &sess)
{
    if(params.size() < 2)
    {
        sCDebug(logSlashCommand) << "Bad invocation:" << String::joined(params," ");
        sendInfoMessage(MessageChannel::USER_ERROR, "Bad invocation:" + String::joined(params, " "), sess);
        return;
    }

    bool ok = true;
    uint32_t delay_status = StringUtils::to_int(params.at(0),&ok);

    if(!ok || delay_status > 2)
    {
        sCDebug(logSlashCommand) << "First argument must be 0, 1, or 2;";
        sendInfoMessage(MessageChannel::USER_ERROR, "First argument must be 0, 1, or 2;", sess);
        return;
    }

    // Combine params after int and use those as door message
    String msg = String::joined(Span<const String>(params).subspan(1)," ");
    sendDoorMessage(sess, DoorMessageStatus(delay_status), msg);
}

void cmdHandler_Browser(const Vector<String> &params, MapClientSession &sess)
{
    String content = String::joined(params, " ");
    sendBrowser(sess, content);
}

void cmdHandler_SendTimeUpdate(const Vector<String> &/*params*/, MapClientSession &sess)
{
    // client expects PostgresEpoch of Jan 1 2000
    DateTime base_date(2000,1,1);
    int32_t time_in_sec = static_cast<int32_t>(base_date.secsTo(DateTime::now()));

    sendTimeUpdate(sess, time_in_sec);
}

void cmdHandler_SendWaypoint(const Vector<String> &params, MapClientSession &sess)
{
    if(params.size() < 3)
    {
        sCDebug(logSlashCommand) << "Bad invocation: " << String::joined(params, " ");
        sendInfoMessage(MessageChannel::USER_ERROR, "Bad invocation:" + String::joined(params, " "), sess);
        return;
    }

    Destination cur_dest = getCurrentDestination(*sess.m_ent);
    int idx = cur_dest.point_idx; // client will only change waypoint if idx == client_side_idx

    glm::vec3 loc {
        StringUtils::to_float(params[0]),
        StringUtils::to_float(params[1]),
        StringUtils::to_float(params[2]),
    };

    String msg = String(String::CtorSprintf(),"Sending SendWaypoint: %d <%0.1f, %0.1f, %0.1f>",
                  idx,
                  loc.x,
                  loc.y,
                  loc.z);


    sendWaypoint(sess, idx, loc);
    setCurrentDestination(*sess.m_ent, idx, loc);
    sendInfoMessage(MessageChannel::USER_ERROR, msg, sess);
}

void cmdHandler_SetStateMode(const Vector<String> &params, MapClientSession &sess)
{
    using namespace magic_enum::bitwise_operators;

    uint32_t val = StringUtils::to_int(params.at(0));

    sess.m_ent->m_state_mode = static_cast<ClientStates>(val);
    sess.m_ent->m_entity_update_flags |= Entity::UpdateFlag::STATEMODE;

    String msg = "Set StateMode to: " + eastl::to_string(val);
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

void cmdHandler_Revive(const Vector<String> &params, MapClientSession &sess)
{
    Entity *tgt = nullptr;
    String msg = "Revive format is '/revive {lvl} {optional: target_name}'";
    if(params.size() < 1)
    {
        sCDebug(logSlashCommand) << msg;
        sendInfoMessage(MessageChannel::USER_ERROR, msg, sess);
        return;
    }

    int revive_lvl = StringUtils::to_int(params.at(0));
    if(params.size() > 1)
    {
        String name = params.at(1);
        tgt = getEntity(&sess, name); // get Entity by name
        if(tgt == nullptr)
        {
            msg = String(String::CtorSprintf(),"Revive target %s cannot be found. Targeting Self.",name.c_str());
            sCDebug(logSlashCommand) << msg;
            sendInfoMessage(MessageChannel::USER_ERROR, msg, sess);
            tgt = sess.m_ent;
        }
    }
    else
        tgt = sess.m_ent;

    revivePlayer(*tgt, static_cast<ReviveLevel>(revive_lvl));

    msg = "Reviving " + tgt->name();
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

void cmdHandler_AddCostumeSlot(const Vector<String> &/*params*/, MapClientSession &sess)
{
    sess.m_ent->m_char->addCostumeSlot();
    markEntityForDbStore(sess.m_ent, DbStoreFlags::Full);

    String msg = "Adding Costume Slot to " + sess.m_ent->name();
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

void cmdHandler_ForceLogout(const Vector<String> &params, MapClientSession &sess)
{
    if(params.size() < 2)
    {
        String msg = "ForceLogout requires a logout message. /forceLogout <HeroName> <Message!>";
        sCDebug(logSlashCommand) << msg;
        sendInfoMessage(MessageChannel::USER_ERROR, msg, sess);
        return;
    }

    String name = params.at(0);
    String message = params.at(1);
    sendForceLogout(sess, name, message);
}

void cmdHandler_SendLocations(const Vector<String> &/*params*/, MapClientSession &sess)
{
    VisitLocation visitlocation;
    visitlocation.m_location_name = "Test1";
    visitlocation.m_pos = glm::vec3(-44, 0, 261);

    sendLocation(sess, visitlocation);
}

void cmdHandler_SendConsoleOutput(const Vector<String> &params, MapClientSession &sess)
{
    if(params.size() < 1)
    {
        sCDebug(logSlashCommand) << "SendConsoleOutput. Bad invocation:  " << String::joined(params, " ");
        sendInfoMessage(MessageChannel::USER_ERROR, "ConsoleOutput format is '/consoleOutput <Message>'", sess);
        return;
    }

    String msg = String::joined(params, " ");
    sendDeveloperConsoleOutput(sess, msg);
}

void cmdHandler_SendConsolePrint(const Vector<String> &params, MapClientSession &sess)
{
    if(params.size() < 1)
    {
        sCDebug(logSlashCommand) << "SendConsolePrintF. Bad invocation:  " << String::joined(params, " ");
        sendInfoMessage(MessageChannel::USER_ERROR, "ConsolePrintF format is '/consolePrintF <Message>'", sess);
        return;
    }

    String msg = String::joined(params, " ");
    sendClientConsoleOutput(sess, msg);
}

void cmdHandler_ClearTarget(const Vector<String> &params, MapClientSession &sess)
{
    if(params.size() != 1)
    {
        sCDebug(logSlashCommand) << "ClearTarget. Bad invocation:  " << String::joined(params, " ");
        sendInfoMessage(MessageChannel::USER_ERROR, "ClearTarget '/clearTarget <targetIdx>'", sess);
        return;
    }

    int idx = StringUtils::to_int(params[0]);
    setTarget(*sess.m_ent, idx);
}

void cmdHandler_StartTimer(const Vector<String> &params, MapClientSession &sess)
{
    if(params.size() != 2)
    {
        sCDebug(logSlashCommand) << "StartTimer. Bad invocation:  " << String::joined(params, " ");
        sendInfoMessage(MessageChannel::USER_ERROR, "StartTimer '/StartTimer <timerName> <seconds>'", sess);
        return;
    }
    String message = params.at(0);
    float time = StringUtils::to_float(params[1]);

    sendMissionObjectiveTimer(sess, message, time);
}

// Slash commands for setting bit values
void cmdHandler_SetU1(const Vector<String> &params, MapClientSession &sess)
{
    uint32_t val = StringUtils::to_int(params.at(0));

    setu1(*sess.m_ent, val);

    String msg = "Set u1 to: " + eastl::to_string(val);
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

//! @}
