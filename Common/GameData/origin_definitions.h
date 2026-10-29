/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once
#include "Common/Containers/String.h"
#include "Common/Containers/Vector.h"

struct Parse_Origin
{
    String Name;
    String DisplayName;
    String DisplayHelp;
    String DisplayShortHelp;
    String Icon;
    int NumBonusPowerSets    = 0;
    int NumBonusPowers       = 0;
    int NumBonusBoostSlots   = 0;
    int NumContacts          = 0;
    float ContactBonusLength = 0.0f;
};
using Parse_AllOrigins = Vector<Parse_Origin>;
