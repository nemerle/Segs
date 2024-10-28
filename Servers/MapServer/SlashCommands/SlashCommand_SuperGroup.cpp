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

#include "SlashCommand_SuperGroup.h"

#include "GameData/Character.h"
#include "GameData/EntityHelpers.h"
#include "Components/Logging.h"
#include "MapInstance.h"
#include "MessageHelpers.h"

#include <QtCore/QString>
#include <QtCore/QDebug>

using namespace SEGSEvents;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Access Level 9 Commands (GMs)
void cmdHandler_SetSuperGroup(const Vector<String> &params, MapClientSession &sess)
{
    int sg_id      = StringUtils::to_int(params.at(0));
    String sg_name = params.at(1);
    int sg_rank    = StringUtils::to_int(params.at(2));

    setSuperGroup(*sess.m_ent, sg_id, sg_name, sg_rank);

    String msg(String::CtorSprintf(),"Set SuperGroup:  id: %d  name: %s  rank: %d",sg_id, sg_name.c_str(), sg_rank);
    sCDebug(logSlashCommand) << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

//! @}
