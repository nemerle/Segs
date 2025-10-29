/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once
#include "Common/GameData/scenegraph_definitions.h"
#include "Common/Runtime/AxisAlignedBox.h"
#include "Common/Runtime/Handle.h"

#include "Components/Colors.h"
#include "EASTL/optional.h"
#include "glm/vec3.hpp"
#include "glm/mat3x3.hpp"
#include "glm/common.hpp"
#include "glm/gtc/constants.hpp"
#include "Common/Containers/HashMap.h"
#include "Common/Containers/Set.h"
#include "EASTL/functional.h"
#include "EASTL/unique_ptr.h"

namespace SEGS
{
class IFilesystem;
struct LoadingContext;
struct Model;
struct SceneNodeChildTransform
{
    struct SceneNode *node;
    glm::mat3x3       m_matrix2;
    glm::vec3         m_pyr;
    glm::vec3         m_translation;
};
struct LightProperties
{
    glm::vec4 color;
    float     range;
    int       is_negative;
};
using HLightProperties = eastl::unique_ptr<LightProperties>;

struct SceneNode
{
    SceneNode(int depth)
    {
        m_nest_level     = depth;
        is_LOD_fade_node = 0;
    }
    ~SceneNode() {
        delete m_properties;
    }
    struct GeoStoreDef                  *m_belongs_to_geoset = nullptr;
    Vector<SceneNodeChildTransform> m_children;
    Vector<GroupProperty_Data> *m_properties = nullptr;
    Vector<eastl::pair<int,String>> m_texture_replacements;
    eastl::optional<eastl::pair<RGBA,RGBA>> tintOverride;

    HLightProperties       m_light;
    Model                 *m_model       = nullptr;
    struct GeoStoreDef    *m_geoset_info = nullptr; // where is this node from ?
    void                  *m_engine_node = nullptr; // used by the engine during loading/importing
    String                 m_name;
    String                 m_dir;
    AxisAlignedBoundingBox m_bbox;
    int                    m_index_in_scenegraph = 0;
    int                    m_nest_level          = 0;
    int                    m_use_count           = 0;

    uint32_t                          m_fx_name_hash = 0; //!< This is fnv1a hash of downcased fx file path.
    glm::vec3                         m_center;
    float                             radius        = 0;
    float                             vis_dist      = 0;
    float                             lod_near      = 0;
    float                             lod_far       = 0;
    float                             lod_near_fade = 0;
    float                             lod_far_fade  = 0;
    float                             lod_scale     = 0;
    float                             shadow_dist   = 0;
    HandleT<20, 12, struct SoundInfo> sound_info;
    bool                              lod_fromtrick = false;
    // Start of bit flags
    uint32_t is_LOD_fade_node : 1;
    uint32_t shell : 1;
    uint32_t tray : 1;
    uint32_t region_marker : 1;
    uint32_t volume_trigger : 1;
    uint32_t water_volume : 1;
    uint32_t lava_volume : 1;
    uint32_t sewer_volume : 1;
    uint32_t door_volume : 1;
    uint32_t in_use : 2;
    uint32_t parent_fade : 1;
    uint32_t key_light : 1;
    // end of bit flags
};

struct RootNode
{
    glm::mat4  mat;
    glm::vec3  pos{0, 0, 0};
    glm::vec3  rot{0, 0, 0};
    SceneNode *node                 = nullptr;
    uint32_t   index_in_roots_array = 0;
};

struct SceneTreeNode
{
    // TODO: REMOVE. This is only used to make debugging dynamic_cast work.
    virtual ~SceneTreeNode() {}
};

struct NodeLoadRequest
{
    String base_file;
    String node_name;
    bool operator==(const NodeLoadRequest& other) const {
        return base_file == other.base_file && node_name == other.node_name;
    }
};
struct NodeLoadTarget
{
    SceneNode *node;
    int        child_idx;
    bool operator==(NodeLoadTarget other) const {
        return node == other.node && child_idx == other.child_idx;
    }
};
}
namespace eastl {
template <>
struct hash<SEGS::NodeLoadRequest>
{
    size_t operator()(const SEGS::NodeLoadRequest & t) const
    {
        return hash<string>()(t.base_file) ^ hash<string>()(t.node_name);
    }
};
}

namespace SEGS {

struct SceneGraph
{
    // Static scene nodes loaded/created from map definition file
    Vector<SceneNode *> all_converted_defs;
    Vector<RootNode *> roots;
    String scene_mod_name;

    HashMap<String,SceneNode *> name_to_node;
    void node_request_instantiation(NodeLoadTarget tgt, NodeLoadRequest needs);
    HashMap<NodeLoadRequest, Vector<NodeLoadTarget>> m_requests;
    ~SceneGraph();
};
struct PrefabStore;
struct LoadingContext;

bool        loadSceneGraph(const String &path, LoadingContext &ctx, PrefabStore &prefabs);
SceneGraph *loadWholeMap(IFilesystem *fs, const String &filename);
SceneGraph* loadSceneGraphNoNesting(IFilesystem* fs, const String& filename, Set<String> &missing_geosets);
void        loadSubgraph(const String &filename, LoadingContext &ctx, PrefabStore &prefabs);
SceneNode  *getNodeByName(const SceneGraph &graph, const String &name);
} // and of SEGS namespace
