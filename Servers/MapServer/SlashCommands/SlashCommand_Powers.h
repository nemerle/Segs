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

// Slash Commands related to Powers

// Access Level 9 Commands (GMs)
void cmdHandler_AddEntirePowerSet(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_AddPower(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_AddInspiration(const Vector<String> &params, MapClientSession &sess);
void cmdHandler_AddEnhancement(const Vector<String> &params, MapClientSession &sess);

// Access Level 1 Commands
void cmdHandler_ReSpec(const Vector<String> &params, MapClientSession &sess);
