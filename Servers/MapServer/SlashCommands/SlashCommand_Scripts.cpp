/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2024 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

/*!
 * @addtogroup SlashCommands Projects/CoX/Servers/MapServer/SlashCommands
 * @{
 */

#include "SlashCommand_Scripts.h"

#include "ScriptingEngine/ScriptingEngine.h"
#include "DataHelpers.h"
#include "Components/Logging.h"
#include "MapInstance.h"
#include "Messages/Map/StandardDialogCmd.h"
#include "MessageHelpers.h"

using namespace SEGSEvents;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Access Level 9 Commands
void cmdHandler_Script(const Vector<String> &params, MapClientSession &sess)
{
    String code = String::joined(params," ");
    sess.m_current_map->m_scripting_interface->runScript(&sess, code, "user provided script");
}

void cmdHandler_SmileX(const Vector<String> &params, MapClientSession &sess)
{
    auto   fs = SEGS::getServiceLocator()->getFS();
    String fileName("scripts/" + String::joined(params," "));
    if(!fileName.ends_with(".smlx"))
        fileName.append(".smlx");
    auto fp = fs->open(fileName, SEGS::IFile::ReadOnly);
    if (fp)
    {
        auto contents(fp->readAll());
        sess.addCommandToSendNextUpdate(eastl::make_unique<StandardDialogCmd>(String(contents.data(),contents.size())));
        delete fp;
    }
    else {
        String errormsg = "Failed to load smilex file. \'" + fileName + "\' not found.";
        sCDebug(logSlashCommand) << errormsg;
        sendInfoMessage(MessageChannel::ADMIN, errormsg, sess);
    }
}

void cmdHandler_ReloadScripts(const Vector<String> &/*params*/, MapClientSession &sess)
{
    sCDebug(logSlashCommand) << "Reloading all Lua scripts in" << sess.m_current_map->name();

    // Reset script engine
    sess.m_current_map->m_scripting_interface.reset(new ScriptingEngine);
    sess.m_current_map->m_scripting_interface->setIncludeDir(sess.m_current_map->name());
    sess.m_current_map->m_scripting_interface->registerTypes();

    // load all scripts again
    // TODO: this will regenerate any NPCs (luabot) that exist
    sess.m_current_map->load_map_lua();
}

//! @}
