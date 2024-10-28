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

#include "SlashCommand_Teams.h"

#include "DataHelpers.h"
#include "GameData/Character.h"
#include "Components/Logging.h"
#include "MapInstance.h"
#include "MessageHelpers.h"
#include "Components/Settings.h"

using namespace SEGSEvents;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Access Level 9 Commands (GMs)
void cmdHandler_SetTeam(const Vector<String> &params, MapClientSession &sess)
{
    uint8_t val = StringUtils::to_int(params.at(0));

    setTeamID(*sess.m_ent, val);

    String msg = "Set Team ID to: " + eastl::to_string(val);
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

void cmdHandler_TeamDebug(const Vector<String> &/*params*/, MapClientSession &sess)
{
    String msg = "Sending team debug to console output.";
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);

    sess.m_ent->m_team->dump(); // Send team debug info
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Access Level 1 Commands
void cmdHandler_Invite(const Vector<String> &params, MapClientSession &sess)
{
    Entity* tgt = getEntity(&sess, String::joined(params," "));
    if(tgt == nullptr)
        return;

    if(tgt->m_has_team)
    {
        const String msg = tgt->name() + " is already on a team.";
        sCDebug(logTeams) << msg;
        sendInfoMessage(MessageChannel::SERVER, msg, sess);
        return;
    }

    if(tgt->name() == sess.m_name)
    {
        const String msg = "You cannot invite yourself to a team.";
        sCDebug(logTeams) << msg;
        sendInfoMessage(MessageChannel::SERVER, msg, sess);
        return;
    }

    if(sess.m_ent->m_has_team && sess.m_ent->m_team != nullptr)
    {
        if(!sess.m_ent->m_team->isTeamLeader(sess.m_ent->m_db_id))
        {
            const String msg = "Only the team leader can invite players to the team.";
            sCDebug(logTeams) << sess.m_ent->name() << msg;
            sendInfoMessage(MessageChannel::TEAM, msg, sess);
            return;
        }
    }

    sendTeamOffer(sess, *tgt->m_client);
}

void cmdHandler_Kick(const Vector<String> &params, MapClientSession &sess)
{
    Entity* tgt = getEntity(&sess, String::joined(params," "));
    if(tgt == nullptr)
        return;

    const String name = tgt->name();
    String msg;
    if(kickTeam(*tgt))
        msg = "Kicking " + name + " from team.";
    else
        msg = "Failed to kick " + name;

    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::TEAM, msg, sess);
}

void cmdHandler_LeaveTeam(const Vector<String> &/*params*/, MapClientSession &sess)
{
    leaveTeam(*sess.m_ent);
    String msg = "Leaving Team";
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::TEAM, msg, sess);
}

void cmdHandler_FindMember(const Vector<String> &/*params*/, MapClientSession &sess)
{
    sendTeamLooking(sess);
    String msg = "Finding Team Member";
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::CHAT_TEXT, msg, sess);
}

void cmdHandler_MakeLeader(const Vector<String> &params, MapClientSession &sess)
{
    Entity* tgt = getEntity(&sess, String::joined(params," "));
    if(tgt == nullptr)
        return;

    const String name = tgt->name();
    String msg;
    if(makeTeamLeader(*sess.m_ent,*tgt))
        msg = "Making " + name + " team leader.";
    else
        msg = "Failed to make " + name + " team leader.";

    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::TEAM, msg, sess);
}

void cmdHandler_TeamBuffs(const Vector<String> & /*params*/, MapClientSession &sess)
{
    toggleTeamBuffs(*sess.m_ent->m_player);

    String msg = "Toggling Team Buffs display mode.";
    sCDebug(logSlashCommand) << msg;
}

// Sidekick Related
void cmdHandler_Sidekick(const Vector<String> &params, MapClientSession &sess)
{
    Entity* tgt = getEntity(&sess, String::joined(params," "));
    if(tgt == nullptr || sess.m_ent->m_char->isEmpty() || tgt->m_char->isEmpty())
        return;

    auto res=inviteSidekick(*sess.m_ent, *tgt);
    static const StringView possible_messages[] = {
        ("Unable to add sidekick."),
        ("To Mentor another player, you must be at least 3 levels higher than them."),
        ("To Mentor another player, you must be at least level 10."),
        ("You are already Mentoring someone."),
        ("Target is already a sidekick."),
        ("To Mentor another player, you must be on the same team."),
    };
    if(res==SidekickChangeStatus::SUCCESS)
    {
        // sendSidekickOffer
        sendSidekickOffer(*tgt->m_client, sess.m_ent->m_db_id); // tgt gets dialog, src.db_id is named.
    }
    else
    {
        sCDebug(logTeams) << possible_messages[int(res)-1];
        sendInfoMessage(MessageChannel::USER_ERROR, possible_messages[int(res)-1], sess);
    }
}

void cmdHandler_UnSidekick(const Vector<String> &/*params*/, MapClientSession &sess)
{
    if(sess.m_ent->m_char->isEmpty())
        return;
    String msg;

    uint32_t sidekick_id = getSidekickId(*sess.m_ent->m_char);
    auto res = removeSidekick(*sess.m_ent, sidekick_id);
    if(res==SidekickChangeStatus::GENERIC_FAILURE)
    {
        msg = "You are not sidekicked with anyone.";
        sCDebug(logTeams) << msg;
        sendInfoMessage(MessageChannel::USER_ERROR, msg, sess);
    }
    else if(res==SidekickChangeStatus::SUCCESS)
    {
        Entity *tgt = getEntityByDBID(sess.m_current_map, sidekick_id);
        String tgt_name = tgt ? tgt->name() : "Unknown Player";
        if(isSidekickMentor(*sess.m_ent))
        {
            // src is mentor, tgt is sidekick
            msg = "You are no longer mentoring "+tgt_name+".";
            sendInfoMessage(MessageChannel::TEAM, msg, sess);
            if(tgt)
            {
                msg = sess.m_ent->name() + " is no longer mentoring you.";
                sendInfoMessage(MessageChannel::TEAM, msg, *tgt->m_client);
            }
        }
        else
        {
            // src is sidekick, tgt is mentor
            if(tgt)
            {
                msg = "You are no longer mentoring " + sess.m_ent->name() + ".";
                sendInfoMessage(MessageChannel::TEAM, msg, *tgt->m_client);
            }
            msg = tgt_name+" is no longer mentoring you.";
            sendInfoMessage(MessageChannel::TEAM, msg, sess);
        }
    }
    else if(res==SidekickChangeStatus::NOT_SIDEKICKED_CURRENTLY)
    {
        msg = "You are no longer sidekicked with anyone.";
        sendInfoMessage(MessageChannel::USER_ERROR, msg, sess);
    }
}

// LFG Related
void cmdHandler_LFG(const Vector<String> &/*params*/, MapClientSession &sess)
{
    toggleLFG(*sess.m_ent);
    String msg = "Toggling LFG";
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::SERVER, msg, sess);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Access Level 0 Commands
void cmdHandler_TeamAccept(const Vector<String> &params, MapClientSession &sess)
{
    // game command: "team_accept \"From\" to_db_id to_db_id \"To\""

    String msgfrom = "Something went wrong with TeamAccept.";
    String msgtgt = "Something went wrong with TeamAccept.";
    String from_name     = params.at(0);
    uint32_t tgt_db_id   = StringUtils::to_int(params.at(1));
    uint32_t tgt_db_id_2 = StringUtils::to_int(params.at(2)); // always the same?
    String tgt_name      = params.at(3);

    if(tgt_db_id != tgt_db_id_2)
        sWarning() << "TeamAccept db_ids do not match!";

    Entity *from_ent = getEntity(&sess,from_name);
    if(from_ent == nullptr)
        return;

    if(inviteTeam(*from_ent,*sess.m_ent))
    {
        msgfrom = "Inviting " + tgt_name + " to team.";
        msgtgt = "Joining " + from_name + "'s team.";

    }
    else
    {
        msgfrom = "Failed to invite " + tgt_name + ". They are already on a team.";
    }

    sCDebug(logSlashCommand) << msgfrom;
    sendInfoMessage(MessageChannel::TEAM, msgfrom, *from_ent->m_client);
    sendInfoMessage(MessageChannel::TEAM, msgtgt, sess);
}

void cmdHandler_TeamDecline(const Vector<String> &params, MapClientSession &sess)
{
    // game command: "team_decline \"From\" to_db_id \"To\""
    String from_name   = params.at(0);
    uint32_t tgt_db_id = StringUtils::to_int(params.at(1));
    String tgt_name    = params.at(2);

    Entity *from_ent = getEntity(&sess,from_name);
    if(from_ent == nullptr)
        return;

    String msg = tgt_name + " declined a team invite from " + from_name + eastl::to_string(tgt_db_id);
    sCDebug(logSlashCommand) << msg;

    msg = tgt_name + " declined your team invite."; // to sender
    sendInfoMessage(MessageChannel::TEAM, msg, *from_ent->m_client);
    msg = "You declined the team invite from " + from_name; // to target
    sendInfoMessage(MessageChannel::TEAM, msg, sess);
}

// Sidekick Related
void cmdHandler_SidekickAccept(const Vector<String> &/*params*/, MapClientSession &sess)
{
    uint32_t db_id  = sess.m_ent->m_char->m_char_data.m_sidekick.m_db_id;
    //TODO: Check that entity is in the same map ?
    Entity *tgt     = getEntityByDBID(sess.m_current_map,db_id);
    if(tgt == nullptr || sess.m_ent->m_char->isEmpty() || tgt->m_char->isEmpty())
        return;

    addSidekick(*sess.m_ent,*tgt);
    // Send message to each player
    String msg = "You are now Mentoring " + tgt->name() + "."; // Customize for src.
    sendInfoMessage(MessageChannel::TEAM, msg, sess);
    msg = sess.m_ent->name()+" is now Mentoring you."; // Customize for src.
    sendInfoMessage(MessageChannel::TEAM, msg, *tgt->m_client);
}

void cmdHandler_SidekickDecline(const Vector<String> &/*params*/, MapClientSession &sess)
{
    sess.m_ent->m_char->m_char_data.m_sidekick.m_db_id = 0;
}

//! @}
