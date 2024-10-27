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

#include "SlashCommand_Powers.h"

#include "DataHelpers.h"
#include "GameData/Character.h"
#include "GameData/CharacterHelpers.h"
#include "Components/Logging.h"
#include "MapInstance.h"
#include "Messages/Map/FloatingInfoStyles.h"
#include "MessageHelpers.h"
#include "Components/Settings.h"

#include <QtCore/QString>
#include <QtCore/QDebug>

using namespace SEGSEvents;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Access Level 9 Commands (GMs)
void cmdHandler_AddEntirePowerSet(const Vector<String> &params, MapClientSession &sess)
{
    if(params.size() < 2)
    {
        sCDebug(logSlashCommand) << "Bad invocation:" << String::joined(params," ");
        sendInfoMessage(MessageChannel::USER_ERROR, "Bad invocation:" + String::joined(params, " "), sess);
        return;
    }

    CharacterData &cd = sess.m_ent->m_char->m_char_data;
    String floating_msg = FloatingInfoMsg.find(FloatingMsg_FoundClue)->second;

    uint32_t v1 = StringUtils::to_int(params.at(0));
    uint32_t v2 = StringUtils::to_int(params.at(1));

    String msg(String::CtorSprintf(), "Granting Entire PowerSet <%d, %d> to %s", v1,v2,sess.m_ent->name().c_str());

    PowerPool_Info ppool;
    ppool.m_pcat_idx = v1;
    ppool.m_pset_idx = v2;

    addEntirePowerSet(cd, ppool);

    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
    sendFloatingInfo(sess, floating_msg, FloatingInfoStyle::FloatingInfo_Attention, 4.0);
}

void cmdHandler_AddPower(const Vector<String> &params, MapClientSession &sess)
{
    if(params.size() < 3)
    {
        sCDebug(logSlashCommand) << "Bad invocation:" << String::joined(params," ");
        sendInfoMessage(MessageChannel::USER_ERROR, "Bad invocation:" + String::joined(params," "), sess);
        return;
    }

    CharacterData &cd = sess.m_ent->m_char->m_char_data;
    String floating_msg = FloatingInfoMsg.find(FloatingMsg_FoundClue)->second;

    uint32_t v1 = StringUtils::to_int(params.at(0));
    uint32_t v2 = StringUtils::to_int(params.at(1));
    uint32_t v3 = StringUtils::to_int(params.at(2));

    String msg(String::CtorSprintf(),"Granting Power <%d, %d, %d> to %s",v1,v2,v3,sess.m_ent->name().c_str());

    PowerPool_Info ppool;
    ppool.m_pcat_idx = v1;
    ppool.m_pset_idx = v2;
    ppool.m_pow_idx = v3;

    addPower(cd, ppool);

    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
    sendFloatingInfo(sess, floating_msg, FloatingInfoStyle::FloatingInfo_Attention, 4.0);
}

void cmdHandler_AddInspiration(const Vector<String> &params, MapClientSession &sess)
{
    String val = String::joined(params," ");
    giveInsp(sess, val);
}

void cmdHandler_AddEnhancement(const Vector<String> &params, MapClientSession &sess)
{
    if(params.empty())
    {
        sCDebug(logSlashCommand) << "Bad invocation:" << String::joined(params," ") << " requires the enhancement name";
        sendInfoMessage(MessageChannel::USER_ERROR, "Bad invocation:" + String::joined(params," ") + " requires the enhancement name", sess);
        return;
    }

    // second param is optional level
    String name = params.at(0);
    uint32_t level = getLevel(*sess.m_ent->m_char);
    if(params.size() > 1)
        level = StringUtils::to_int(params.at(1)) -1;

    giveEnhancement(sess, name, level);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Access Level 1 Commands
void cmdHandler_ReSpec(const Vector<String> &/*params*/, MapClientSession &sess)
{
    CharacterData &cd = sess.m_ent->m_char->m_char_data;

    if(sess.m_ent->m_char->isEmpty())
        return;

    String msg = "No powersets found for player " + sess.m_ent->name();

    if(cd.m_powersets.size() > 1)
    {
        msg = "Removing all powers for player " + sess.m_ent->name();
        cd.m_reset_powersets = true;
        cd.m_has_updated_powers = true;
    }

    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
    sCDebug(logSlashCommand) << msg;
}


//! @}
