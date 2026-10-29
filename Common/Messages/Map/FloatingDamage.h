/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once
#include "GameCommand.h"
#include "MapEventTypes.h"
#include "Components/BitStream.h"

namespace SEGSEvents
{

// [[ev_def:type]]
class FloatingDamage final : public GameCommandEvent
{
public:
    // [[ev_def:field]]
    int m_damage_source = 0;
    // [[ev_def:field]]
    int m_damage_target = 0;
    // [[ev_def:field]]
    int m_amount = 0; // should be float?

    explicit FloatingDamage() : GameCommandEvent(evFloatingDamage) {}
    FloatingDamage(int source,int target,int amount) : GameCommandEvent(evFloatingDamage),
        m_damage_source(source),
        m_damage_target(target),
        m_amount(amount)
    {
    }

    void serializeto(BitStream &bs) const override
    {
        bs.StorePackedBits(1,type()-evFirstServerToClient); // packet 21

        bs.StorePackedBits(1,m_damage_source);
        bs.StorePackedBits(1,m_damage_target);
        bs.StorePackedBits(1,m_amount);
    }
    void serializefrom(BitStream &bs) override
    {
        m_damage_source = bs.GetPackedBits(1);
        m_damage_target = bs.GetPackedBits(1);
        m_amount = bs.GetPackedBits(1);
    }
    EVENT_IMPL(FloatingDamage)
};

} //end of SEGSEvents namespace
