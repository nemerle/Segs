/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once
#include "GameCommand.h"
#include "MapEventTypes.h"
#include "FloatingInfoStyles.h"
#include "Components/BitStream.h"

namespace SEGSEvents
{
// [[ev_def:type]]
class FloatingInfo final : public GameCommandEvent
{
public:
                        // [[ev_def:field]]
    uint32_t            m_tgt_idx = 0;
                        // [[ev_def:field]]
    String              m_msg;
                        // [[ev_def:field]]
    FloatingInfoStyle   m_style = FloatingInfo_Damage;
                        // [[ev_def:field]]
    float               m_delay = 0.0f;
    explicit            FloatingInfo() : GameCommandEvent(evFloatingInfo) {}
                        FloatingInfo(uint32_t tgt_idx, const String &msg, FloatingInfoStyle style, float delay) : GameCommandEvent(evFloatingInfo),
                            m_tgt_idx(tgt_idx),
                            m_msg(msg),
                            m_style(style),
                            m_delay(delay)
                        {
                        }

        void            serializeto(BitStream &bs) const override
                        {
                            bs.StorePackedBits(1, type()-evFirstServerToClient); // 56

                            bs.StorePackedBits(1, m_tgt_idx);
                            bs.StoreString(m_msg);
                            bs.StorePackedBits(2, m_style);
                            bs.StoreFloat(m_delay);
                        }
        void            serializefrom(BitStream &bs) override
                        {
                            m_tgt_idx = bs.GetPackedBits(1);
                            bs.GetString(m_msg);
                            m_style = FloatingInfoStyle(bs.GetPackedBits(2));
                            m_delay = bs.GetFloat();
                        }
                        EVENT_IMPL(FloatingInfo)
};
} // end of SEGSEvents namespace
