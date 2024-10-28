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

#include "SlashCommand_Stats.h"

#include "DataHelpers.h"
#include "GameData/Character.h"
#include "GameData/CharacterHelpers.h"
#include "Components/Logging.h"
#include "MapInstance.h"
#include "Messages/Map/FloatingInfoStyles.h"
#include "MessageHelpers.h"
#include "Components/Settings.h"

using namespace SEGSEvents;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Access Level 9 Commands (GMs)
void cmdHandler_Falling(const Vector<String> &/*params*/, MapClientSession &sess)
{
    toggleFalling(*sess.m_ent);

    String msg = "Toggling falling";
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

void cmdHandler_Sliding(const Vector<String> &/*params*/, MapClientSession &sess)
{
    toggleSliding(*sess.m_ent);

    String msg = "Toggling sliding";
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

void cmdHandler_Jumping(const Vector<String> &/*params*/, MapClientSession &sess)
{
    toggleJumping(*sess.m_ent);

    String msg = "Toggling jumping";
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

void cmdHandler_Stunned(const Vector<String> &/*params*/, MapClientSession &sess)
{
    toggleStunned(*sess.m_ent);

    String msg = "Toggling stunned";
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

void cmdHandler_SetSpeed(const Vector<String> &params, MapClientSession &sess)
{
    float v1 = StringUtils::to_float(params.at(0));
    float v2 = StringUtils::to_float(params.at(1));
    float v3 = StringUtils::to_float(params.at(2));
    setSpeed(*sess.m_ent, v1, v2, v3);

    String msg(String::CtorSprintf(),"Set Speed to: <%d,%d,%d>",v1,v2,v3);

    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

void cmdHandler_SetBackupSpd(const Vector<String> &params, MapClientSession &sess)
{
    float val = StringUtils::to_float(params.at(0));
    setBackupSpd(*sess.m_ent, val);

    String msg = "Set BackupSpd to: " + eastl::to_string(val);
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

void cmdHandler_SetJumpHeight(const Vector<String> &params, MapClientSession &sess)
{
    float val = StringUtils::to_float(params.at(0));
    setJumpHeight(*sess.m_ent, val);

    String msg = "Set JumpHeight to: " + eastl::to_string(val);
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

void cmdHandler_SetHP(const Vector<String> &params, MapClientSession &sess)
{
    float attrib = StringUtils::to_float(params.at(0));

    changeHP(*sess.m_ent, attrib);

    String msg = String(String::CtorSprintf(),"Setting HP to: %f / %f",attrib,getMaxHP(*sess.m_ent->m_char));
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

void cmdHandler_SetEnd(const Vector<String> &params, MapClientSession &sess)
{
    float attrib = StringUtils::to_float(params.at(0));
    float maxattrib = sess.m_ent->m_char->m_max_attribs.m_Endurance;

    if(attrib > maxattrib)
        attrib = maxattrib;

    setEnd(*sess.m_ent->m_char,attrib);

    String msg(String::CtorSprintf(),"Setting Endurance to: %f / %f",attrib,maxattrib);
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

void cmdHandler_SetXP(const Vector<String> &params, MapClientSession &sess)
{
    uint32_t attrib = StringUtils::to_int(params.at(0));
    uint32_t lvl = getLevel(*sess.m_ent->m_char);

    setXP(*sess.m_ent->m_char, attrib);
    String msg = "Setting XP to " + eastl::to_string(attrib);

    uint32_t newlvl = getLevel(*sess.m_ent->m_char);
    if(lvl != newlvl)
        msg += " and LVL to " + eastl::to_string(newlvl);

    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

void cmdHandler_GiveXP(const Vector<String> &params, MapClientSession &sess)
{
    uint32_t attrib = StringUtils::to_int(params.at(0));
    uint32_t lvl = getLevel(*sess.m_ent->m_char);

    giveXp(sess, attrib);
    String msg = "Giving " + eastl::to_string(attrib) + " XP";

    uint32_t newlvl = getLevel(*sess.m_ent->m_char);
    if(lvl != newlvl)
        msg += " and setting LVL to " + eastl::to_string(newlvl);

    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

void cmdHandler_SetDebt(const Vector<String> &params, MapClientSession &sess)
{
    uint32_t attrib = StringUtils::to_int(params.at(0));

    setDebt(*sess.m_ent->m_char, attrib);
    String msg(String::CtorSprintf(),"Setting XP Debt to %d",attrib);

    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

void cmdHandler_SetInf(const Vector<String> &params, MapClientSession &sess)
{
    uint32_t attrib = StringUtils::to_int(params.at(0));

    setInf(*sess.m_ent->m_char, attrib);

    String msg = "Setting influence to: " + eastl::to_string(attrib);
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

void cmdHandler_SetLevel(const Vector<String> &params, MapClientSession &sess)
{
    uint32_t attrib = StringUtils::to_int(params.at(0)) - 1; // convert from 1-50 to 0-49

    setLevel(*sess.m_ent->m_char, attrib);

    String contents = FloatingInfoMsg.find(FloatingMsg_Leveled)->second;
    sendFloatingInfo(sess, contents, FloatingInfoStyle::FloatingInfo_Attention, 4.0);

    String msg = "Setting Level to: " + eastl::to_string(attrib + 1);
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

void cmdHandler_SetCombatLevel(const Vector<String> &params, MapClientSession &sess)
{
    uint32_t attrib = StringUtils::to_int(params.at(0)) - 1; // convert from 1-50 to 0-49

    setCombatLevel(*sess.m_ent->m_char, attrib);

    String msg = "Setting Combat Level to: " + eastl::to_string(attrib + 1);
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Access Level 2 Commands
void cmdHandler_Alignment(const Vector<String> &params, MapClientSession &sess)
{
    if(params.size() == 1)
    {
        setAlignment(*sess.m_ent, params.at(0));
        sendInfoMessage(MessageChannel::DEBUG_INFO, "New alignment: " + params.at(0), sess);
        return;
    }
    String msg = "Choose from hero, villain, both or none/neither: ";
    sCDebug(logSlashCommand) << msg << String::joined(params," ");
    sendInfoMessage(MessageChannel::USER_ERROR, msg + String::joined(params," "), sess);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Access Level 1 Commands
void cmdHandler_AFK(const Vector<String> &params, MapClientSession &sess)
{
    Entity* e = sess.m_ent;
    String afk_msg = String::joined(params," ");
    toggleAFK(*e->m_char, afk_msg);

    String msg = "Setting afk message to: " + afk_msg;
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::EMOTE, msg, sess);

    // the server regards writing on chat (including cmd commands) as an input
    // so specifically for afk, we treat it as a non-input in this way
    e->m_has_input_on_timeframe = false;
}

void cmdHandler_SetTitles(const Vector<String> &params, MapClientSession &sess)
{
    String title = String::joined(params," ");
    setTitle(sess, title);
}

void cmdHandler_SetCustomTitles(const Vector<String> &params, MapClientSession &sess)
{
    String     msg;

    if(params.size() == 0)
    {
        setTitles(*sess.m_ent->m_char);
        msg = "Titles reset to nothing";
    }
    else
    {
        bool   prefix  = !params.at(0).empty();
        String generic = params.at(1);
        String origin  = params.at(2);
        String special = params.at(3);
        setTitles(*sess.m_ent->m_char, prefix, generic, origin, special);
        msg = "Titles changed to: " + eastl::to_string(prefix) + " " + generic + " " + origin + " " + special;
    }
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::USER_ERROR, msg, sess);
}

void cmdHandler_SetSpecialTitle(const Vector<String> &params, MapClientSession &sess)
{
    bool   prefix  = sess.m_ent->m_char->m_char_data.m_has_the_prefix;
    String generic = getGenericTitle(*sess.m_ent->m_char);
    String origin  = getOriginTitle(*sess.m_ent->m_char);
    String special = String::joined(params," ");

    setTitles(*sess.m_ent->m_char, prefix, generic, origin, special);
    String msg = "Titles changed to: " + eastl::to_string(prefix) + " " + generic + " " + origin + " " + special;

    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::USER_ERROR, msg, sess);
}

void cmdHandler_SetAssistTarget(const Vector<String> &/*params*/, MapClientSession &sess)
{
    // it appears that `/assist` should target the target of your current target
    // but it also seems that what we call m_assist_target_idx is actually
    // the last friendly target you selected. We need to check target type
    // and set m_target_idx or m_assist_target_idx depending on result.

    Entity *target_ent = getTargetEntity(sess);
    if(target_ent == nullptr)
        return;

    uint32_t new_target = getTargetIdx(*target_ent);
    if(new_target == 0)
        return;

    if(target_ent->m_is_villain)
        setTarget(*sess.m_ent, new_target);
    else
        setAssistTarget(*sess.m_ent, new_target);

    String msg = "Now targeting " + target_ent->name() + "'s target";
    sendInfoMessage(MessageChannel::TEAM, msg, sess);
    sCDebug(logSlashCommand) << msg;
}

//! @}
