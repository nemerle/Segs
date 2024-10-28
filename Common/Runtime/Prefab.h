/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once
#include "Components/serialization_common.h"

#include "Common/Containers/HashMap.h"
#include "Common/Containers/Vector.h"
#include "Common/Containers/String.h"
#include "Common/Containers/Set.h"


namespace SEGS
{
struct NodeLoadRequest;
struct Model;
struct SceneGraph;
struct SceneNode;

struct GeoSet
{
    String          geopath;
    String          name;
    Vector<Model *> subs;
    Vector<String>  tex_names;
    Vector<char>    m_geo_data;
    uint32_t             geo_data_size;
    bool                 data_loaded = false;
    ~GeoSet();
};
Model *getModelById(GeoSet *gset, int id);

// Geo file info
struct GeoStoreDef
{
    String geopath;        //!< a path to a .geo file
    Vector<String> entries;    //!< the names of models contained in a geoset
    bool loaded;
};
struct NameList
{
    HashMap<String, String> new_names; // map from old node name to a new name
    String basename;
};

struct LoadingContext
{
    LoadingContext(int depth) : m_nesting_level(depth) {}
    NameList m_renamer; // used to rename prefab nodes to keep all names unique in the loaded graph
    String m_base_path;
    SceneGraph* m_target;
    IFilesystem *fs_wrap;
    int last_node_id=0; // used to create new number suffixes for generic nodes
    int m_nesting_level=0; // how deep are we in include hierarchy
    bool prevent_nesting=false;

};
struct PrefabStore
{
    HashMap<String, GeoStoreDef> m_dir_to_geoset;
    HashMap<String, GeoStoreDef *> m_modelname_to_geostore;
    Set<String> m_missing_geosets;
    IFilesystem *m_fs;
    String m_base_path;

    PrefabStore(IFilesystem* fs,const String &bp) : m_fs(fs),m_base_path(bp) {}
    ~PrefabStore();

    bool prepareGeoLookupArray(const String &base_path);
    bool loadPrefabForNode(SceneNode *node, LoadingContext &ctx);
    bool loadNamedPrefab(const String &name, LoadingContext &conv, NodeLoadRequest *load_request=nullptr);
    Model *groupModelFind(const String &path, LoadingContext &ctx);
    Model *modelFind(const String &geoset_name, const String &model_name, LoadingContext &ctx);
    GeoStoreDef * groupGetFileEntryPtr(const String &full_name);
    void sceneGraphWasReset(); // reset 'loaded' flag on all geostores
};

} // namespace SEGS
