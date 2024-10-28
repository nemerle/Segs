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

// Slash Commands related to Character Stats or values

// Access Level 9 Commands (GMs)
void cmdHandler_Falling(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_Sliding(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_Jumping(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_Stunned(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_SetSpeed(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_SetBackupSpd(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_SetJumpHeight(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_SetHP(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_SetEnd(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_SetXP(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_GiveXP(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_SetDebt(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_SetInf(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_SetLevel(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_SetCombatLevel(const Vector<String> &params, MapClientSession &sess);

// Access Level 2[GM] Commands
void cmdHandler_Alignment(const Vector<String> &params, MapClientSession &sess);

// Access Level 1 Commands
void cmdHandler_AFK(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_SetTitles(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_SetCustomTitles(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_SetSpecialTitle(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_SetAssistTarget(const Vector<String> &params, MapClientSession &sess);
