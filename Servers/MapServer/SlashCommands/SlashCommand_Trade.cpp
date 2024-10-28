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

#include "SlashCommand_Trade.h"

#include "DataHelpers.h"
#include "GameData/Trade.h"
#include "Components/Logging.h"
#include "MapInstance.h"
#include "MessageHelpers.h"
#include "Components/Settings.h"

using namespace SEGSEvents;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Access Level 1 Commands
void cmdHandler_Trade(const Vector<String> &params, MapClientSession &sess)
{
    Entity* tgt = getEntity(&sess, String::joined(params," "));
    if(tgt == nullptr || sess.m_ent == nullptr)
        return;

    String msg, tgt_msg;
    TradeSystemMessages result = requestTrade(*sess.m_ent, *tgt);

    switch(result)
    {
    case TradeSystemMessages::SRC_ALREADY_IN_TRADE:
        msg = "You are already in a trade.";
        break;
    case TradeSystemMessages::SRC_CONSIDERING_ANOTHER_TRADE:
        msg = "You are already considering a trade offer.";
        break;
    case TradeSystemMessages::TGT_ALREADY_IN_TRADE:
        msg = tgt->name() + " is already in a trade.";
        break;
    case TradeSystemMessages::TGT_CONSIDERING_ANOTHER_TRADE:
        msg = tgt->name() + " is already considering a trade offer.";
        break;
    case TradeSystemMessages::SEND_TRADE_OFFER:
        sendTradeOffer(*tgt->m_client, sess.m_ent->name()); // send tradeOffer
        msg = "You sent a trade request to "+tgt->name()+".";
        tgt_msg = sess.m_ent->name() + " sent a trade request.";
        sendInfoMessage(MessageChannel::SERVER, tgt_msg, *tgt->m_client);
        break;
    default:
        msg = "Something went wrong with trade request!"; // this should never happen
    }

    sendInfoMessage(MessageChannel::SERVER, msg, *sess.m_ent->m_client);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Access Level 0 Commands
void cmdHandler_TradeAccept(const Vector<String> &params, MapClientSession &sess)
{
    // Game command: "trade_accept \"From\" to_db_id \"To\""
    if(params.size() < 3)
    {
        sWarning() << "Wrong number of arguments for TradeAccept.";
        discardTrade(*sess.m_ent);
        return;
    }

    // We need only the "from" name.
    const String from_name = params.at(0);
    Entity* const from_ent = getEntity(&sess, from_name);
    if(from_ent == nullptr)
    {
        discardTrade(*sess.m_ent);
        return;
    }

    String msg, tgt_msg;
    TradeSystemMessages result = acceptTrade(*sess.m_ent, *from_ent);

    switch(result)
    {
    case TradeSystemMessages::HAS_SENT_NO_TRADE:
        msg = "You have not sent a trade offer.";
        break;
    case TradeSystemMessages::TGT_RECV_NO_TRADE:
        msg = from_ent->name()+" has not received a trade offer.";
        break;
    case TradeSystemMessages::SRC_RECV_NO_TRADE:
        msg = "You are not considering a trade offer from " + from_ent->name() + ".";
        break;
    case TradeSystemMessages::ACCEPTED_TRADE:
        sendTradeInit(*sess.m_ent->m_client, *from_ent->m_client); // Initiate trade
        msg = "You accepted the trade invite from "+from_ent->name()+".";
        tgt_msg = sess.m_ent->name() + " accepted your trade invite.";
        sendInfoMessage(MessageChannel::SERVER, tgt_msg, *from_ent->m_client);
        break;
    default:
        msg = "Something went wrong with trade accept!"; // this should never happen
    }

    sendInfoMessage(MessageChannel::SERVER, msg, sess);
}

void cmdHandler_TradeDecline(const Vector<String> &params, MapClientSession &sess)
{
    // Game command: "trade_decline \"From\" to_db_id \"To\""
    if(params.size() < 3)
    {
        sWarning() << "Wrong number of arguments for TradeDecline.";
        discardTrade(*sess.m_ent);
        return;
    }

    // We need only the "from" name.
    const String from_name  = params.at(0);
    Entity* const from_ent = getEntity(&sess, from_name);
    if(from_ent == nullptr)
    {
        discardTrade(*sess.m_ent);
        return;
    }

    String msg, tgt_msg;
    TradeSystemMessages result = declineTrade(*sess.m_ent, *from_ent);

    switch(result)
    {
    case TradeSystemMessages::HAS_SENT_NO_TRADE:
        msg = "You have not sent a trade offer.";
        break;
    case TradeSystemMessages::TGT_RECV_NO_TRADE:
        msg = from_ent->name()+" has not received a trade offer.";
        break;
    case TradeSystemMessages::SRC_RECV_NO_TRADE:
        msg = "You are not considering a trade offer from "+from_ent->name()+".";
        break;
    case TradeSystemMessages::DECLINED_TRADE:
        msg     = "You declined the trade invite from " + from_ent->name()+".";
        tgt_msg = sess.m_ent->name() + " declined your trade invite.";
        sendInfoMessage(MessageChannel::SERVER, tgt_msg, *from_ent->m_client);
        break;
    default:
        msg = "Something went wrong with trade decline!"; // this should never happen
    }

    sendInfoMessage(MessageChannel::SERVER, msg, sess);
}

//! @}
