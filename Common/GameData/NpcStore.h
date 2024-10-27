/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once
#include "Common/GameData/npc_definitions.h"
#include <Common/Containers/HashMap.h>
#include <Common/Containers/StringView.h>

struct NPCStorage
{
    AllNpcs_Data m_all_npcs;
    HashMap<String,Parse_NPC *> m_name_to_npc_def;
    void prepare_dictionaries();
    int npc_idx(const Parse_NPC *npc) const;
    int npc_idx(const String &name);
    const Parse_NPC *npc_by_name(StringView name) const;
};
