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

#include "SlashCommand_Misc.h"

#include "DataHelpers.h"
#include "GameData/CharacterHelpers.h"
#include "Components/Logging.h"
#include "MapInstance.h"
#include "MessageHelpers.h"
#include "Components/Settings.h"

using namespace SEGSEvents;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Access Level 2 Commands
void cmdHandler_AddNPC(const Vector<String> &params, MapClientSession &sess)
{
    if(params.size() < 1)
    {
        String err = "Bad invocation:" + String::joined(params," ");
        sCDebug(logSlashCommand) << err;
        sendInfoMessage(MessageChannel::USER_ERROR, err, sess);
        return;
    }
    String name = params.at(0);
    // Variation may not be supplied, so default to 0
    int variation = StringUtils::to_int(params.at(1));

    glm::vec3 offset = glm::vec3 {2,0,1};
    glm::vec3 gm_loc = sess.m_ent->m_entity_data.m_pos + offset;
    addNpc(sess, name, gm_loc, variation, name);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Access Level 1 Commands
void cmdHandler_WhoAll(const Vector<String> &/*params*/, MapClientSession &sess)
{
    MapInstance *     mi  = sess.m_current_map;

    String msg = "Players on this map:\n";

    for (MapClientSession *cl : mi->m_session_store)
    {
        Character &c(*cl->m_ent->m_char);
        String    name      = cl->m_ent->name();
        String    lvl       = eastl::to_string(getLevel(c)+1);         //+1 as the server stores these values
        String     clvl     = eastl::to_string(getCombatLevel(c) + 1); // with a 0 index, issue #831
        String    origin    = getOrigin(c);
        String    archetype = String(getClass(c)).replaced("Class_","");

        // Format: character_name "lvl" level "clvl" combat_level origin archetype
        msg += String(String::CtorSprintf(), "%s lvl %s clvl %s %s %s\n", name.c_str(), lvl.c_str(), clvl.c_str(),
                      origin.c_str(), archetype.c_str());
    }

    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::SERVER, msg, sess);
}

void cmdHandler_MOTD(const Vector<String> &/*params*/, MapClientSession &sess)
{
    sendServerMOTD(&sess);
    String msg = "Opening Server MOTD";
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::SERVER, msg, sess);
}

void cmdHandler_Tailor(const Vector<String> &/*params*/, MapClientSession &sess)
{
    sendTailorOpen(sess);
}

void cmdHandler_CostumeChange(const Vector<String> &params, MapClientSession &sess)
{
    uint32_t costume_idx = StringUtils::to_int(params.at(0));

    setCurrentCostumeIdx(*sess.m_ent->m_char, costume_idx);

    sCDebug(logTailor) << "Changing costume to: " << costume_idx;
}

void cmdHandler_Train(const Vector<String> &/*params*/, MapClientSession &sess)
{
    playerTrain(sess);
}

void cmdHandler_Kiosk(const Vector<String> &/*params*/, MapClientSession &sess)
{
    sendKiosk(sess);
}


//! @}
