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

#include "SlashCommand_Settings.h"

#include "DataHelpers.h"
#include "GameData/playerdata_definitions.h"
#include "GameData/gui_definitions.h"
#include "Components/Logging.h"
#include "MapInstance.h"
#include "MessageHelpers.h"
#include "Components/Settings.h"

#include <QtCore/QString>
#include <QtCore/QDebug>

using namespace SEGSEvents;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Access Level 9 Commands
void cmdHandler_SettingsDump(const Vector<String> &/*params*/, MapClientSession &sess)
{
    String msg = "Sending settings config dump to console output.";
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);

    settingsDump(); // Send settings dump
}

void cmdHandler_GUIDebug(const Vector<String> &/*params*/, MapClientSession &sess)
{
    String msg = "Sending GUISettings dump to console output.";
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);

    sess.m_ent->m_player->m_gui.guiDump(); // Send GUISettings dump
}

void cmdHandler_SetWindowVisibility(const Vector<String> &params, MapClientSession &sess)
{
    uint32_t idx = StringUtils::to_int(params.at(0));
    WindowVisibility val = (WindowVisibility)StringUtils::to_int(params.at(1));

    String msg = "Toggling " + eastl::to_string(idx) + " GUIWindow visibility: " + eastl::to_string(val);
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);

    sess.m_ent->m_player->m_gui.m_wnds.at(idx).setWindowVisibility(val); // Set WindowVisibility
    sess.m_ent->m_player->m_gui.m_wnds.at(idx).guiWindowDump(); // for debugging
}

void cmdHandler_KeybindDebug(const Vector<String> &/*params*/, MapClientSession &sess)
{
    String msg = "Sending Keybinds dump to console output.";
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);

    sess.m_ent->m_player->m_keybinds.keybindsDump(); // Send GUISettings dump
}

void cmdHandler_ToggleLogging(const Vector<String> &params, MapClientSession &sess)
{
    String msg = "Toggle logging of categories: " + String::joined(params," ");
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);

    for (auto category : params)
        toggleLogging(category); // Toggle each category listed
}

//! @}
