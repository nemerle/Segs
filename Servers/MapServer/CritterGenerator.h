/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once

#include "Containers/HashMap.h"
#include "GameData/spawn_definitions.h"

struct CritterGenerator
{
    String                             m_generator_name;
    String                             m_encounter_node_name;
    CritterSpawnLocations              m_critter_encounter;
    Vector<CritterSpawnDef>            m_possible_critters_and_groups;
    void generate(class MapInstance *);
};

struct CritterGeneratorStore
{
    HashMap<String, CritterGenerator> m_generators;
    void generate(class MapInstance *instance);
};

String makeReadableName(const String &name);
