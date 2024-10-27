/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2020 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once
#include "Containers/String.h"
#include "Containers/Vector.h"

struct MapClientSession;

// Slash Commands related to Teams, Sidekicks, and LFG

// Access Level 9 Commands (GMs)
void cmdHandler_SetTeam(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_TeamDebug(const Vector<String> &params, MapClientSession &sess);

// Access Level 1 Commands
void cmdHandler_Invite(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_Kick(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_LeaveTeam(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_FindMember(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_MakeLeader(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_TeamBuffs(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_Sidekick(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_UnSidekick(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_LFG(const Vector<String> &params, MapClientSession &sess);

// Access Level 0 Commands
void cmdHandler_TeamAccept(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_TeamDecline(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_SidekickAccept(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_SidekickDecline(const Vector<String> &params, MapClientSession &sess);
