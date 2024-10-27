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

#include "SlashCommand_Movement.h"

#include "DataHelpers.h"
#include "Components/Logging.h"
#include "MapInstance.h"
#include "MapLink.h"
#include "Messages/Map/MapXferWait.h"
#include "MessageHelpers.h"
#include "Components/Settings.h"

using namespace SEGSEvents;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Access Level 9 Commands (GMs)
void cmdHandler_ControlsDisabled(const Vector<String> &/*params*/, MapClientSession &sess)
{
    toggleControlsDisabled(*sess.m_ent);

    String msg = "Toggling controls";
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

void cmdHandler_UpdateId(const Vector<String> &params, MapClientSession &sess)
{
    uint8_t attrib = StringUtils::to_int(params.at(0));

    setUpdateID(*sess.m_ent, attrib);

    String msg = "Setting updateID to: " + eastl::to_string(attrib);
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

void cmdHandler_FullUpdate(const Vector<String> &/*params*/, MapClientSession &sess)
{
    toggleFullUpdate(*sess.m_ent);

    String msg = "Toggling full update";
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

void cmdHandler_HasControlId(const Vector<String> &/*params*/, MapClientSession &sess)
{
    toggleControlId(*sess.m_ent);

    String msg = "Toggling has control id";
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

void cmdHandler_ToggleInterp(const Vector<String> &/*params*/, MapClientSession &sess)
{
    toggleInterp(*sess.m_ent);

    String msg = "Toggling interpolation";
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

void cmdHandler_ToggleMoveInstantly(const Vector<String> &/*params*/, MapClientSession &sess)
{
    toggleMoveInstantly(*sess.m_ent);

    String msg = "Toggling instant movement";
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

void cmdHandler_ToggleCollision(const Vector<String> &/*params*/, MapClientSession &sess)
{
    toggleCollision(*sess.m_ent);

    String msg = "Toggling collision";
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

void cmdHandler_ToggleMovementAuthority(const Vector<String> &/*params*/, MapClientSession &sess)
{
    toggleMovementAuthority(*sess.m_ent);

    String msg = "Toggling server authority for movement";
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

void cmdHandler_FaceEntity(const Vector<String> &params, MapClientSession &sess)
{
    Entity *tgt = nullptr;

    String name = String::joined(params," ");

    if(params.size() < 1)
    {
        sCDebug(logSlashCommand) << "Bad invocation:" << name;
        sendInfoMessage(MessageChannel::USER_ERROR, "Bad invocation:" + name, sess);
        return;
    }

    tgt = getEntity(&sess, name); // get Entity by name
    if(tgt == nullptr)
    {
        String msg(String::CtorSprintf(),"FaceEntity target %s cannot be found.",name.c_str());
        sCDebug(logSlashCommand) << msg;
        sendInfoMessage(MessageChannel::USER_ERROR, msg, sess);
        return;
    }
    sendFaceEntity(sess, tgt->m_idx);
}

void cmdHandler_FaceLocation(const Vector<String> &params, MapClientSession &sess)
{
    if(params.size() < 3)
    {
        sCDebug(logSlashCommand) << "Bad invocation:" << String::joined(params," ");
        sendInfoMessage(MessageChannel::USER_ERROR, "Bad invocation:" + String::joined(params," "), sess);
        return;
    }

    glm::vec3 loc {
      StringUtils::to_float(params.at(0)),
      StringUtils::to_float(params.at(1)),
      StringUtils::to_float(params.at(2))
    };

    sendFaceLocation(sess, loc);
}

void cmdHandler_MoveZone(const Vector<String> &params, MapClientSession &sess)
{
    uint32_t map_idx = StringUtils::to_int(params.at(0));
    if(map_idx == getMapIndex(sess.m_current_map->name()))
        map_idx = (map_idx + 1) % 39;   // To prevent crashing if trying to access the map you're on.
    MapXferData map_data = MapXferData();
    map_data.m_target_map_name = getMapName(map_idx);
    sess.link()->putq(new MapXferWait(getMapPath(map_idx)));

    HandlerLocator::getMap_Handler(sess.is_connected_to_game_server_id)
        ->putq(new ClientMapXferMessage({sess.link()->session_token(), map_data}, 0));
}

void cmdHandler_ToggleInputLog(const Vector<String> &params, MapClientSession &sess)
{
    String name = sess.m_ent->name();
    if(!params.empty())
        name = String::joined(params," ");

    // getEntityByNameOrTarget will always return a valid Entity, or self
    Entity* target = getEntityByNameOrTarget(sess, name);
    target->m_input_state.m_debug = !target->m_input_state.m_debug;
}

void cmdHandler_ToggleMovementLog(const Vector<String> &params, MapClientSession &sess)
{
    String name = sess.m_ent->name();
    if(!params.empty())
        name = String::joined(params," ");

    // getEntityByNameOrTarget will always return a valid Entity, or self
    Entity* target = getEntityByNameOrTarget(sess, name);
    target->m_motion_state.m_debug = !target->m_motion_state.m_debug;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Access Level 2 Commands
void cmdHandler_MoveTo(const Vector<String> &params, MapClientSession &sess)
{
    if(params.size() < 3)
    {
        String errormsg = "Bad invocation. /moveto expects 3 parameters e.g. '/moveto x y z'. Received:" + String::joined(params," ");
        sCDebug(logSlashCommand) << errormsg;
        sendInfoMessage(MessageChannel::USER_ERROR, errormsg, sess);
        return;
    }

    glm::vec3 new_pos {
      StringUtils::to_float(params.at(0)),
      StringUtils::to_float(params.at(1)),
      StringUtils::to_float(params.at(2))
    };
    forcePosition(*sess.m_ent, new_pos);
    sendInfoMessage(MessageChannel::DEBUG_INFO, String("New position set"), sess);
}

void cmdHandler_Fly(const Vector<String> &/*params*/, MapClientSession &sess)
{
    toggleFlying(*sess.m_ent);

    String msg = "Toggling flight";
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

void cmdHandler_Jumppack(const Vector<String> &/*params*/, MapClientSession &sess)
{
    toggleJumppack(*sess.m_ent);

    String msg = "Toggling jumppack";
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

void cmdHandler_Teleport(const Vector<String> &params, MapClientSession &sess)
{
    String msg = "Teleport format is '/teleport {target_name}'";
    String name = sess.m_ent->name();
    if(!params.empty())
        name = String::joined(params," ");

    // getEntityByNameOrTarget will always return a valid Entity, or self
    Entity *tgt = getEntityByNameOrTarget(sess, name);
    name = tgt->name(); // make sure we have the final name
    glm::vec3 new_pos = tgt->m_entity_data.m_pos;

    sendFaceLocation(sess, new_pos);
    forcePosition(*sess.m_ent, new_pos);

    msg = String("Teleporting to target " + name + ".");
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Access Level 1 Commands
void cmdHandler_Stuck(const Vector<String> &params, MapClientSession &sess)
{
    // TODO: Implement true move-to-safe-location-nearby logic
    //forcePosition(*sess.m_ent, sess.m_current_map->closest_safe_location(sess.m_ent->m_entity_data.m_pos));
    sess.m_current_map->setPlayerSpawn(*sess.m_ent);

    String msg(String::CtorSprintf(),"Resetting location to default spawn <%0.1f, %0.1f, %0.1f>",
            sess.m_ent->m_entity_data.m_pos.x,
            sess.m_ent->m_entity_data.m_pos.y,
            sess.m_ent->m_entity_data.m_pos.z);

    sCDebug(logSlashCommand) << String::joined(params," ") << ":" << msg;
    sendInfoMessage(MessageChannel::SERVER, msg, sess);
}

void cmdHandler_SetSpawnLocation(const Vector<String> &params, MapClientSession &sess)
{
    if(params.size() == 0)
    {
        // No SpawnLocation given, bail.
        return;
    }
    const String spawnLocation = String::joined(params," ");
    sess.m_current_map->setSpawnLocation(*sess.m_ent, spawnLocation);
}

void cmdHandler_MapXferList(const Vector<String> &/*params*/, MapClientSession &sess)
{
    showMapMenu(sess);
}

//! @}
