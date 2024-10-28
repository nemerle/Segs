/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once
#include <stdint.h>
#include "Common/Containers/Vector.h"

struct Parse_Combining
{
    Vector<float> CombineChances;
};

struct Parse_Effectiveness
{
    Vector<float> Effectiveness;
};

struct LevelExpAndDebt
{
    Vector<uint32_t> m_ExperienceRequired;
    Vector<uint32_t> m_DefeatPenalty;
};

struct Parse_PI_Schedule
{
    Vector<uint32_t> m_FreeBoostSlotsOnPower;
    Vector<uint32_t> m_PoolPowerSet;
    Vector<uint32_t> m_Power;
    Vector<uint32_t> m_AssignableBoost;
    Vector<uint32_t> m_InspirationCol;
    Vector<uint32_t> m_InspirationRow;
    Vector<uint32_t> m_BoostSlot;
};
