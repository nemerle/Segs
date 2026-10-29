/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once

#include "GameCommand.h"
#include "MapEvents.h"

namespace SEGSEvents
{

// [[ev_def:type]]
class FaceLocation : public GameCommandEvent
{
public:
    explicit FaceLocation() : GameCommandEvent(evFaceLocation) {}
    FaceLocation(glm::vec3 loc) : GameCommandEvent(evFaceLocation),
        m_loc(loc)
    {}

    // SerializableEvent interface
    void serializefrom(BitStream &bs) override
    {
        m_loc.x = bs.GetFloat();
        m_loc.y = bs.GetFloat();
        m_loc.z = bs.GetFloat();
    }
    void serializeto(BitStream &bs) const override
    {
        bs.StorePackedBits(1, type()-evFirstServerToClient); // pkt 55
        bs.StoreFloat(m_loc.x);
        bs.StoreFloat(m_loc.y);
        bs.StoreFloat(m_loc.z);
    }
    EVENT_IMPL(FaceLocation)

    // [[ev_def:field]]
    glm::vec3 m_loc = glm::vec3(0.0f);
};

} //end of SEGSEvents namespace
