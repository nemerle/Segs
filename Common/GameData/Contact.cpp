/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#include "Contact.h"

#include "Components/Logging.h"
#include "Components/serialization_common.h"
#include "Components/serialization_types.h"

HashMap<String,uint32_t> contactLinkHash = {
    {"CONTACTLINK_HELLO"                ,1},
    {"CONTACTLINK_MAIN"                 ,2},
    {"CONTACTLINK_BYE"                  ,3},
    {"CONTACTLINK_MISSIONS"             ,4},
    {"CONTACTLINK_LONGMISSION"          ,5},
    {"CONTACTLINK_SHORTMISSION"         ,6},
    {"CONTACTLINK_ACCEPTLONG"           ,7},
    {"CONTACTLINK_ACCEPTSHORT"          ,8},
    {"CONTACTLINK_INTRODUCE"            ,9},
    {"CONTACTLINK_INTRODUCE_CONTACT1"   ,0x0A},
    {"CONTACTLINK_INTRODUCE_CONTACT2"   ,0x0B},
    {"CONTACTLINK_ACCEPT_CONTACT1"      ,0x0C},
    {"CONTACTLINK_ACCEPT_CONTACT2"      ,0x0D},
    {"CONTACTLINK_GOTOSTORE"            ,0x0E},
    {"CONTACTLINK_TRAIN"                ,0x0F},
    {"CONTACTLINK_WRONGMODE"            ,0x10},
    {"CONTACTLINK_DONTKNOW"             ,0x11},
    {"CONTACTLINK_NOTLEADER"            ,0x12},
    {"CONTACTLINK_BADCELLCALL"          ,0x13},
    {"CONTACTLINK_ABOUT"                ,0x14},
    {"CONTACTLINK_IDENTIFYCLUE"         ,0x15},
    {"CONTACTLINK_NEWPLAYERTELEPORT_AP" ,0x16},
    {"CONTACTLINK_NEWPLAYERTELEPORT_GC" ,0x17},
    {"CONTACTLINK_FORMTASKFORCE"        ,0x18},
    {"CONTACTLINK_CHOOSE_TITLE"         ,0x19},
    {"CONTACTLINK_GOTOTAILOR"           ,0x1A},
};


template<class Archive>
void Contact::serialize(Archive &archive, uint32_t const version)
{
    if(version != Contact::class_version)
    {
        sCritical() << "Failed to serialize Contact, incompatible serialization format version " << version;
        return;
    }

    archive(cereal::make_nvp("Name",m_name));
    archive(cereal::make_nvp("LocationDescription",m_location_description));
    archive(cereal::make_nvp("npcId",m_npc_id));
    archive(cereal::make_nvp("ContactIdx",m_contact_idx));
    archive(cereal::make_nvp("CurrentStanding",m_current_standing));
    archive(cereal::make_nvp("ConfidantThreshold",m_confidant_threshold));
    archive(cereal::make_nvp("FriendThreshold",m_friend_threshold));
    archive(cereal::make_nvp("CompleteThreshold",m_complete_threshold));
    archive(cereal::make_nvp("TaskIndex",m_task_index));
    archive(cereal::make_nvp("NotifyPlayer",m_notify_player));
    archive(cereal::make_nvp("CanUseCell",m_can_use_cell));
    archive(cereal::make_nvp("HasLocation",m_has_location));
    archive(cereal::make_nvp("Location",m_location));
}

CEREAL_CLASS_VERSION(Contact, Contact::class_version)   // register Contact class version
SPECIALIZE_CLASS_VERSIONED_SERIALIZATIONS(Contact)

template<class Archive>
void Destination::serialize(Archive &archive, uint32_t const version)
{
    if(version != Destination::class_version)
    {
        sCritical() << "Failed to serialize Destination, incompatible serialization format version " << version;
        return;
    }

    archive(cereal::make_nvp("PointIdx",point_idx));
    archive(cereal::make_nvp("Location",location));
    archive(cereal::make_nvp("LocationName",m_location_name));
    archive(cereal::make_nvp("LocationMapName",m_location_map_name));
}

CEREAL_CLASS_VERSION(Destination, Destination::class_version)   // register Destination class version
SPECIALIZE_CLASS_VERSIONED_SERIALIZATIONS(Destination)
