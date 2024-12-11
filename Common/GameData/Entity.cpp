/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

/*!
 * @addtogroup GameData Projects/CoX/Common/GameData
 * @{
 */

#include "Entity.h"
#include "EntityHelpers.h"
#include "LFG.h"
#include "Team.h"
#include "Character.h"
#include "CharacterHelpers.h"
#include "GameData/GameDataStore.h"
#include "GameData/playerdata_definitions.h"
#include "GameData/npc_definitions.h"
#ifdef SEGS_STANDALONE
#include "Servers/MapServer/DataHelpers.h"
#endif
//#include <QtCore/QDebug>
#include "EASTL/algorithm.h"
#include <cmath>

void Entity::sendAllyID(BitStream &bs)
{
    bs.StorePackedBits(2,0);
    bs.StorePackedBits(4,0); // NPC->0
}

void Entity::sendPvP(BitStream &bs)
{
    bs.StoreBits(1,0);
    bs.StoreBits(1,0);
    bs.StorePackedBits(5,0);
    bs.StoreBits(1,0);
}

void Entity::fillFromCharacter(const GameDataStore &data)
{
    m_hasname = !m_char->getName().empty();
    m_entity_data.m_origin_idx = getEntityOriginIndex(data,true, getOrigin(*m_char));
    m_entity_data.m_class_idx = getEntityClassIndex(data,true, getClass(*m_char));
    m_is_hero = true;
}

/**
 *  This will mark the Entity as being in logging out state
 *  \arg time_till_logout is time in seconds untill logout is done
 */
void Entity::beginLogout(uint16_t time_till_logout)
{
    using namespace magic_enum::bitwise_operators;
    m_is_logging_out = true;
    m_entity_update_flags |= UpdateFlag::LOGOUT;
    m_time_till_logout = time_till_logout*1000;
}

const String &Entity::name() const
{
    return m_char->getName();
}

void Entity::dump()
{
    String msg = "EntityDebug\n  "
            + name()
            + "\n  db_id: " + eastl::to_string(m_db_id)
            + "\n  entity idx: " + eastl::to_string(m_idx)
            + "\n  access level: " + eastl::to_string(m_entity_data.m_access_level)
            + "\n  m_type: " + eastl::to_string(uint8_t(m_type))
            + "\n  class idx: " + eastl::to_string(m_entity_data.m_class_idx)
            + "\n  origin idx: " + eastl::to_string(m_entity_data.m_origin_idx)
            + "\n  mapidx: " + eastl::to_string(m_entity_data.m_map_idx)
            + "\n  pos: " + eastl::to_string(m_entity_data.m_pos.x) + ", "
                          + eastl::to_string(m_entity_data.m_pos.y) + ", "
                          + eastl::to_string(m_entity_data.m_pos.z)
            + "\n  orient: " + eastl::to_string(m_entity_data.m_orientation_pyr.p) + ", "
                             + eastl::to_string(m_entity_data.m_orientation_pyr.y) + ", "
                             + eastl::to_string(m_entity_data.m_orientation_pyr.r)
            + "\n  target: " + eastl::to_string(m_target_idx)
            + "\n  assist target: " + eastl::to_string(m_assist_target_idx)
            + "\n  m_SG_id: " + eastl::to_string(m_supergroup.m_SG_id);

    sDebug() << msg;

    if(m_team != nullptr)
        m_team->dump();

    if(m_type == EntType::PLAYER || m_type == EntType::NPC)
        m_char->dump();
    if(m_type == EntType::PLAYER)
        m_player->dump();
    dumpFriends(*this);
}

void Entity::setActiveDialogCallback(eastl::function<void(int)> callback)
{
    this->m_active_dialog = callback;
}

Entity::Entity()
{
}

Entity::~Entity()
{

}

//! @}
