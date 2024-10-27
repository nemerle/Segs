/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once
#include "EASTL/unique_ptr.h"
#include "Common/GameData/map_definitions.h"
#include "Common/GameData/spawn_definitions.h"
#include "Common/Containers/HashMap.h"

#include <glm/mat4x4.hpp>

namespace SEGS
{
struct SceneGraph;
struct SceneNode;
} // namespace SEGS

///
/// \brief The MapSceneGraph class and functions operating on it are central point of access to the world's geometry
///
class MapSceneGraph
{
    eastl::unique_ptr<SEGS::SceneGraph> m_scene_graph;
    //! Contains all nodes from the scene graph that have any properties set, for faster lookups.
    //! @todo consider creating a property-name => [SceneNode,SceneNode] mapping instead ?
    Vector<SEGS::SceneNode *> m_nodes_with_properties;
public:
    MapSceneGraph();
    ~MapSceneGraph();
    bool                                         loadFromFile(const String &mapname);
    eastl::unordered_multimap<String, glm::mat4> getSpawnPoints() const;
    HashMap<String, MapXferData>                 get_map_transfers() const;
    void                                         spawn_npcs(class MapInstance *instance);
    void                                         build_combat_navigation_graph();
    void                                         build_pedestrian_navigation_graph();
    Vector<SpawnerNode>                          m_csNodes;
    Vector<SpawnerNode>                          m_persNodes;
    Vector<SpawnerNode>                          m_carNodes;
    Vector<SpawnerNode>                          m_npcNodes;
};

String getCostumeFromName(const String &n);
