/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

/*!
 * @addtogroup SlashCommands Projects/CoX/Servers/MapServer/SlashCommands
 * @{
 */

#include "SlashCommand_Email.h"

#include "DataHelpers.h"
#include "GameData/Character.h"
#include "Components/Logging.h"
#include "MapInstance.h"
#include "MessageHelpers.h"

using namespace SEGSEvents;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Access Level 0 Commands
void cmdHandler_EmailHeaders(const Vector<String> & /*params*/, MapClientSession &sess)
{
    getEmailHeaders(sess);
}

void cmdHandler_EmailRead(const Vector<String> &params, MapClientSession &sess)
{
    uint32_t id = StringUtils::to_int(params.at(0));

    readEmailMessage(sess, id);
}

void cmdHandler_EmailSend(const Vector<String> &params, MapClientSession &sess)
{
    if (params.size() < 3)
    {
        sendInfoMessage(MessageChannel::SERVER, "Argument count for sending email is not correct! Please send emails from the email window instead.", sess);
        return;
    }

    // params are: recipient name, email subject, email message words
    String recipients = params.at(0);
    // recipients from email window are enclosed in \q
    recipients.replace("\\q ", ";");
    recipients.replace("\\q", "");
    Vector<String> recipient_list = recipients.split(';');
    // the last element will be empty if sent through email window, so remove it
    if (recipient_list.back().empty())
    {
        recipient_list.pop_back();
    }

    // first, check if your own character is one of the recipients in the email
    // cannot send email to self as that will trigger /emailRead without the data in db nor EmailHandler
    // and that will segfault the server :)
    if (recipients.contains(sess.m_ent->m_char->getName()))
    {
        sendInfoMessage(MessageChannel::SERVER, "You cannot send an email to yourself!", sess);
        return;
    }

    for (const auto &recipient : recipient_list)
        // Everything after recipient and subject is the email body
        sendEmail(sess, recipient, params.at(1), String::joined(Span<const String>(params).subspan(2)," "));
}

void cmdHandler_EmailDelete(const Vector<String> &params, MapClientSession &sess)
{
    uint32_t id = StringUtils::to_int(params.at(0));

    deleteEmailHeaders(sess, id);

    String msg = "Email Deleted ID: " + eastl::to_string(id);
    sDebug() << msg;
    sendInfoMessage(MessageChannel::DEBUG_INFO, msg, sess);
}

//! @}
