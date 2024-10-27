/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once

#include "Common/Containers/String.h"
#include "Common/Containers/Vector.h"
#include "Common/Containers/StringView.h"

#include <glm/vec3.hpp>

// Move SpawnerNode to MapSceneGraph and remove spawn_definitions.cpp/.h?
// CritterGenerator.cpp/.h now also obselete?
class SpawnerNode
{
    public:
    String              m_name;         // Nodes name
    Vector<SpawnerNode> m_markers;      // Children nodes, typically spawn markers
    glm::vec3           m_position;     // definite world position
    glm::vec3           m_rotation;     // definite rotation
    bool operator==(const SpawnerNode &) const = default;
};

// Everything below this point obselete?
class CritterSpawnPoint
{
public:
    String          m_name;
    bool            m_is_victim;
    glm::vec3       m_relative_position;
    glm::vec3       m_rotation;

    StringView getName() const { return m_name;}
    void setName(const char *n) { m_name = n; }
};

class CritterSpawnLocations
{
public:
    String                                      m_node_name;
    Vector<CritterSpawnPoint>              m_all_spawn_points;
    uint8_t                                     m_spawn_probability;
    uint8_t                                     m_villain_radius; // Aggro range?

    StringView getNodeName() const { return m_node_name;}
    void setNodeName(const char *n) { m_node_name = n; }
};

struct CritterDefinition
{
    String      m_model;
    String      m_name;
    String      m_faction_name;
    bool        m_spawn_all;
};

struct CritterSpawnDef
{
   String                          m_spawn_group;
   Vector<CritterDefinition>       m_possible_critters;
};

class SpawnDefinitions
{
public:

    SpawnDefinitions()                  { buildList();}
    CritterSpawnDef         getSpawnGroup(const String &spawn_group_name);
    Vector<CritterSpawnDef> getCritterSpawnDefinitions();
    void                                buildList();

private:
    Vector<CritterSpawnDef> m_critter_spawn_list;
};
