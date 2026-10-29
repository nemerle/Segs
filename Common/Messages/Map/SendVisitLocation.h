/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once

#include "GameCommand.h"
#include "MapEventTypes.h"
#include "Common/GameData/VisitLocation.h"

namespace SEGSEvents
{
    // [[ev_def:type]]
    class SendLocations final : public GameCommandEvent
    {

    public:
        // [[ev_def:field]
        vLocationList m_locations;
        SendLocations() : GameCommandEvent(MapEventTypes::evSendLocations){}
        explicit SendLocations(vLocationList &&location_list) : GameCommandEvent(MapEventTypes::evSendLocations)
        {
            m_locations = eastl::move(location_list);
        }

        void serializeto(BitStream &bs) const override
        {
            //Not sure what the purpose of this packet is.
            //Could be tied to glowies or plagues/location/early badges
            bs.StorePackedBits(1, type()-evFirstServerToClient); // packet 63
            sCDebug(logMapEvents) << "SendLocations Event serializeTo. m_locations.size(): " << m_locations.size();
            bs.StorePackedBits(1, (uint32_t)m_locations.size());

            for(const VisitLocation &location : m_locations)
            {
               bs.StoreString(location.m_location_name);
               sCDebug(logMapEvents) << "SendLocations Event serializeTo. m_location_name: " << location.m_location_name;
            }
        }

        void serializefrom(BitStream &bs) override
        {
            m_locations.clear();
            const uint32_t count = bs.GetPackedBits(1);
            for(uint32_t i = 0; i < count && bs.GetReadableBits() > 0; ++i)
            {
                VisitLocation location;
                bs.GetString(location.m_location_name);
                m_locations.push_back(eastl::move(location));
            }
        }

        EVENT_IMPL(SendLocations)
    };
}


