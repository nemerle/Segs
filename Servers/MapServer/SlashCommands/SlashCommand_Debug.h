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

// Slash Commands related to Debugging

// Access Level 9 Commands (GMs)
void cmdHandler_Dialog(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_InfoMessage(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_DebugChar(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_UpdateChar(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_SendFloatingNumbers(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_SetSequence(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_AddTriggeredMove(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_AddTimeStateLog(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_SetClientState(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_LevelUpXp(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_TestDeadNoGurney(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_DoorMessage(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_Browser(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_SendTimeUpdate(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_SendWaypoint(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_SetStateMode(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_Revive(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_AddCostumeSlot(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_ForceLogout(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_SendLocations(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_SendConsoleOutput(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_SendConsolePrint(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_ClearTarget(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_StartTimer(const Vector<String> &params, MapClientSession &sess);

// For live value-testing
void cmdHandler_SetU1(const Vector<String> &params, MapClientSession &sess);
