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
    class Browser : public GameCommandEvent
    {
    public:
        //  [[ev_def:field]]
        String m_content;

        explicit Browser() : GameCommandEvent(MapEventTypes::evBrowser) {}
        Browser(const String &content) : GameCommandEvent(MapEventTypes::evBrowser),
            m_content(content)
        {
        }

        void serializeto(BitStream &bs) const override
        {
            bs.StorePackedBits(1, type()-MapEventTypes::evFirstServerToClient); // 76
            bs.StoreString(m_content);
        }
        EVENT_IMPL(Browser)
    };

    // [[ev_def:type]]
    class BrowserClose final : public MapLinkEvent
    {
    public:
        BrowserClose() : MapLinkEvent(MapEventTypes::evBrowserClose)
        {
        }
        void    serializeto(BitStream &bs) const override
        {
            bs.StorePackedBits(1,type()-MapEventTypes::evFirstServerToClient); // 77
        }
        void    serializefrom(BitStream &/*bs*/) override
        {
            sCDebug(logMapEvents) << "Browser Close Event";
        }

        EVENT_IMPL(BrowserClose)
    };

} // end of SEGSEvents namespace
