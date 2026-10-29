/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once
#include "GameData/LFG.h"
#include "MapEventTypes.h"
#include "GameCommand.h"
#include "Components/BitStream.h"
#include "Components/Logging.h"

namespace SEGSEvents
{
// [[ev_def:type]]
class TeamLooking final : public GameCommandEvent
{
public:
    // [[ev_def:field]]
    uint32_t m_num = 0;
    // [[ev_def:field]]
    Vector<LFGMember> m_list;
    explicit TeamLooking() : GameCommandEvent(MapEventTypes::evTeamLooking) {}
    TeamLooking(const Vector<LFGMember> &list) : GameCommandEvent(MapEventTypes::evTeamLooking),
        m_num((uint32_t)list.size()),
        m_list(list)
    {
    }
    void    serializeto(BitStream &bs) const override {
        bs.StorePackedBits(1,type()-MapEventTypes::evFirstServerToClient); // 25

        bs.StoreBits(32,m_num); // size of list
        for(const auto &m : m_list)
        {
            bs.StoreString(m.m_name);
            bs.StoreString(m.m_classname);
            bs.StoreString(m.m_origin);
            bs.StoreBits(32,m.m_level);
        }

        if(logLFG().isDebugEnabled())
            dumpLFGList();
    }
    void    serializefrom(BitStream &bs) override {
        m_list.clear();
        m_num = uint32_t(bs.GetBits(32));
        for(uint32_t i = 0; i < m_num && bs.GetReadableBits() > 0; ++i)
        {
            LFGMember m;
            m.m_db_id = 0;
            bs.GetString(m.m_name);
            bs.GetString(m.m_classname);
            bs.GetString(m.m_origin);
            m.m_level = uint32_t(bs.GetBits(32));
            m_list.push_back(eastl::move(m));
        }
    }
    EVENT_IMPL(TeamLooking)
};
} // end of SEGSEvents namespace

