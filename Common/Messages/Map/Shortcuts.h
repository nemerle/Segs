/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once
// if this file is included stand-alone this will pull in common definitions
#include "MapEventTypes.h"
#include "MapSessionReference.h"
#include "Components/BitStream.h"

namespace SEGSEvents
{
// [[ev_def:type]]
class Shortcuts : public MapLinkEvent
{
public:
    explicit Shortcuts():MapLinkEvent(MapEventTypes::evShortcuts) {}
    Shortcuts(MapClientSession *sess):MapLinkEvent(MapEventTypes::evShortcuts),m_session({0,sess})
    {
    }
    void serializeto(BitStream &bs) const override
    {
        bs.StorePackedBits(1,4); // opcode
        for(uint32_t i=0; i<m_commands.size(); i++)
        {
            bs.StorePackedBits(1,i+1);
            bs.StoreString(m_commands[i]);
        }
        bs.StorePackedBits(1,~0u);
        bs.StorePackedBits(1,uint32_t(m_server_commands.size()));
        for(const String &cmd : m_server_commands)
            bs.StoreString(cmd);
    }
    void serializefrom(BitStream &bs) override
    {
        m_commands.clear();
        for(;;)
        {
            int idx = bs.GetPackedBits(1);
            if(idx<0 || bs.getLastError())
                break;
            String name;
            bs.GetString(name);
            if(idx>0 && idx<=2000)
            {
                m_commands.resize(std::max<size_t>(m_commands.size(),size_t(idx)));
                m_commands[idx-1] = name;
            }
        }
        int num = bs.GetPackedBits(1);
        m_server_commands.clear();
        for(int i=0; i<num && !bs.getLastError(); i++)
        {
            String name;
            bs.GetString(name);
            m_server_commands.push_back(name);
        }
    }
    // [[ev_def:field]]
    SessionReference m_session;
    // [[ev_def:field]]
    Vector<String> m_commands;
    // [[ev_def:field]]
    Vector<String> m_server_commands;
    EVENT_IMPL(Shortcuts)
};
} // end of SEGSEvents namespace

