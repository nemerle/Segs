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
class EmailHeaders final : public GameCommandEvent
{
public:
    struct EmailHeader
    {
        uint32_t id;
        String sender;
        String subject;
        uint32_t timestamp;
        template<class Archive>
        void serialize(Archive &ar)
        {
            ar(id,sender,subject,timestamp);
        }
    };

    EmailHeaders() : GameCommandEvent(MapEventTypes::evEmailHeaders) {}
    /*Send multiple emails*/
    explicit EmailHeaders(const Vector<EmailHeader> &email) : GameCommandEvent(MapEventTypes::evEmailHeaders),
        m_fullupdate(true),
        m_emails(email)
    {
    }

    /*Defines a single email header to send*/
    EmailHeaders(const uint32_t &id, const String &sender, const String &subject, const uint32_t &timestamp)
        : GameCommandEvent(evEmailHeaders), m_fullupdate(false)
    {
        m_emails.push_back(EmailHeader{ id, sender, subject, timestamp });
    }

    void serializeto(BitStream &bs) const override
    {
        bs.StorePackedBits(1, type()-evFirstServerToClient);
        bs.StorePackedBits(1, m_fullupdate);
        bs.StorePackedBits(1, (uint32_t)m_emails.size());

        for(const EmailHeader &hdr : m_emails)
        {
            bs.StoreBits(1, true); //"valid" flag
            bs.StoreBits(32, hdr.id);
            bs.StoreString(hdr.sender);
            bs.StoreString(hdr.subject);
            bs.StoreBits(32, hdr.timestamp);
        }
    }

    void serializefrom(BitStream &bs) override
    {
        m_emails.clear();
        m_fullupdate = bs.GetPackedBits(1) != 0;
        const uint32_t count = bs.GetPackedBits(1);
        for(uint32_t i = 0; i < count && bs.GetReadableBits() > 0; ++i)
        {
            EmailHeader hdr;
            (void)bs.GetBits(1); // "valid" flag
            hdr.id = uint32_t(bs.GetBits(32));
            bs.GetString(hdr.sender);
            bs.GetString(hdr.subject);
            hdr.timestamp = uint32_t(bs.GetBits(32));
            m_emails.push_back(eastl::move(hdr));
        }
    }
    // [[ev_def:field]]
    bool    m_fullupdate = false; //Forces a refresh of the email window
    // [[ev_def:field]]
    Vector<EmailHeader> m_emails;

    EVENT_IMPL(EmailHeaders)
};

} // end of SEGSEvents namespace

