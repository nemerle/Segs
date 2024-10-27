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

// Slash Commands related to Movement

// Access Level 9 Commands (GMs)
void cmdHandler_ControlsDisabled(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_UpdateId(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_FullUpdate(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_HasControlId(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_ToggleInterp(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_ToggleMoveInstantly(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_ToggleCollision(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_ToggleMovementAuthority(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_FaceEntity(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_FaceLocation(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_MoveZone(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_ToggleInputLog(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_ToggleMovementLog(const Vector<String> &params, MapClientSession &sess);

// Access Level 2[GM] Commands
void cmdHandler_MoveTo(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_Fly(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_Jumppack(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_Teleport(const Vector<String> &params, MapClientSession &sess);

// Access Level 1 Commands
void cmdHandler_Stuck(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_SetSpawnLocation(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_MapXferList(const Vector<String> &params, MapClientSession &sess);
