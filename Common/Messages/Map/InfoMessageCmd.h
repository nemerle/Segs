/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once
#include "GameCommand.h"
#include "MapEvents.h"

enum class MessageChannel : int;

namespace SEGSEvents
{

// [[ev_def:type]]
class InfoMessageCmd : public GameCommandEvent
{
public:
    // [[ev_def:field]]
    MessageChannel  m_channel_type = MessageChannel(0);
    // [[ev_def:field]]
    String         m_msg;

explicit InfoMessageCmd() : GameCommandEvent(evInfoMessageCmd) {}
    InfoMessageCmd(MessageChannel ch, StringView msg) : GameCommandEvent(evInfoMessageCmd),
        m_channel_type(ch),
        m_msg(msg)
    {
    }
    void serializeto(BitStream &bs) const override
    {
        bs.StorePackedBits(1,type()-MapEventTypes::evFirstServerToClient); // packet 15
        bs.StorePackedBits(2, static_cast<uint8_t>(m_channel_type));
        bs.StoreString(m_msg);
    }
    void serializefrom(BitStream &bs) override
    {
        m_channel_type = static_cast<MessageChannel>(bs.GetPackedBits(2));
        bs.GetString(m_msg);
    }
    EVENT_IMPL(InfoMessageCmd)
};

} // end of SEGSEvents namespace
