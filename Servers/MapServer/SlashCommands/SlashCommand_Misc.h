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

// Misc. Slash Commands

// Access Level 2[GM] Commands
void cmdHandler_AddNPC(const Vector<String> &params, MapClientSession &sess);

// Access Level 1 Commands
void cmdHandler_WhoAll(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_MOTD(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_Tailor(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_CostumeChange(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_Train(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_Kiosk(const Vector<String> &params, MapClientSession &sess);
