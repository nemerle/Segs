#include "Prefab.h"

#include "Common/GameData/GameDataStore.h"
#include "Common/GameData/scenegraph_serializers.h" //for getFilepathCaseInsensitive
#include "Common/GameData/trick_serializers.h"
#include "Common/Utils/IServiceLocator.h"
#include "Components/Logging.h"
#include "Model.h"
#include "SceneGraph.h"
#include "Utils/string_utils.h"

namespace SEGS
{
GeoSet::~GeoSet() {
    for(Model *m : subs)
        delete m;
}
}

HashMap<String,SEGS::GeoSet *> s_name_to_geoset;

using namespace SEGS;

GeoSet *findAndPrepareGeoSet(IFilesystem *fs,const String &fname,const String &base_path)
{
    GeoSet *geoset = nullptr;
    String name_fixed = fname;
    name_fixed.replace(".anm", ".geo");
    String true_path = getFilepathCaseInsensitive(fs,base_path + name_fixed);

    IFile *fp = fs->open(true_path,IFile::ReadOnly);
    if(fp)
    {
        geoset = new GeoSet;
        //TODO: QDir(base_path).relativeFilePath(true_path) should be provided by fs service.
        geoset->geopath = PathUtils::path_to(base_path,true_path);
        geosetLoadHeader(fp, geoset);
        fp->seek(0);
        s_name_to_geoset[fname] = geoset;
        delete fp;
    }
    else
        sCritical() << "Can't find .geo file" << fname;

    return geoset;
}

/// load the given geoset, used when loading scene-subgraph and nodes
GeoSet * geosetLoad(IFilesystem *fs, const String &m, const String &base_path)
{
    GeoSet * res = s_name_to_geoset.at(m,nullptr);
    if(res)
        return res;

    return findAndPrepareGeoSet(fs,m,base_path);
}

Model *PrefabStore::modelFind(const String &geoset_name, const String &model_name, LoadingContext &ctx)
{
    Model *ptr_sub = nullptr;
    if(model_name.empty() || geoset_name.empty())
    {
        sCritical() << "Bad model/geometry set requested:";
        if(!model_name.empty())
            sCritical() << "Model: " << model_name;
        if(!geoset_name.empty())
            sCritical() << "GeoFile: " << geoset_name;
        return nullptr;
    }

    GeoSet *geoset = geosetLoad(ctx.fs_wrap,geoset_name, m_base_path);
    if(!geoset) { // failed to load the geometry set
        m_missing_geosets.insert(geoset_name);
        return nullptr;
    }

    auto end_of_name_idx = model_name.find("__");
    if(end_of_name_idx == String::npos)
        end_of_name_idx = model_name.size();

    StringView basename(StringView(model_name).substr(0, end_of_name_idx));

    for(Model *m : geoset->subs)
    {
        StringView geo_name = m->name;
        if(geo_name.empty())
            continue;

        bool subs_in_place = (geo_name.size() <= end_of_name_idx || geo_name.substr(end_of_name_idx).starts_with("__"));
        if(subs_in_place && StringUtils::begins_with(geo_name,basename, StringUtils::CaseInsensitive))
            ptr_sub = m; // TODO: return immediately
    }

    return ptr_sub;
}

bool PrefabStore::prepareGeoLookupArray(const String &base_path)
{
    auto services=getServiceLocator();
    auto fs=services->getFS();
    String bin_path =base_path + "bin/defnames.bin";
    auto file=fs->open(bin_path,IFile::ReadOnly);
    if(!file)
    {
        sCritical() << "Failed to open bin/defnames.bin:" << bin_path;
        return false;
    }

    GeoStoreDef *current_geosetinf = nullptr;
    auto data=file->readAll();
    Vector<StringView> defnames_arr;
    String::split_ref(defnames_arr, StringView(data.data(),data.size()), '\0');
    for(StringView &str : defnames_arr)
    {
        if(str=="CHUNKS.geo")
            str = StringView("Chunks.geo");
    }
    for(StringView str : defnames_arr)
        {
        auto last_slash = str.rfind('/');
        if(String::npos != last_slash)
        {
            String geo_path(str.substr(0, last_slash));
            String lookup_str          = geo_path.to_lower();
            current_geosetinf          = &m_dir_to_geoset[lookup_str];
            current_geosetinf->geopath = eastl::move(geo_path);
        }
        current_geosetinf->entries.emplace_back(str.substr(last_slash + 1));
        m_modelname_to_geostore[String(str.substr(last_slash + 1))] = current_geosetinf;
    }

    delete file;
    return true;
}

bool PrefabStore::loadPrefabForNode(SceneNode *node, LoadingContext &ctx) //groupLoadRequiredLibsForNode
{
    GeoStoreDef *gf;

    if(!node || !node->in_use)
        return false;

    if(node->m_geoset_info)
        gf = node->m_geoset_info;
    else
    {
        gf = groupGetFileEntryPtr(node->m_name);
        node->m_geoset_info = gf;
        if(!node->m_geoset_info)
            node->m_geoset_info = (GeoStoreDef *)-1; // prevent future load attempts
    }

    if(!gf || gf == (GeoStoreDef *)-1)
        return false;

    if(!gf->loaded)
    {
        gf->loaded = true;
        geosetLoad(ctx.fs_wrap,gf->geopath, m_base_path); // load given subgraph's root geoset
        loadSubgraph(gf->geopath,ctx,*this);
    }

    return true;
}
bool PrefabStore::loadNamedPrefab(const String &name, LoadingContext &ctx, NodeLoadRequest* load_request) //groupFileLoadFromName
{
    GeoStoreDef *geo_store = groupGetFileEntryPtr(name);
    if(!geo_store)
        return false;
    if(ctx.prevent_nesting)
    {
        if(load_request)
        {
            String geofi(geo_store->geopath);
            StringView base_file = PathUtils::path(geofi);
            load_request->base_file = base_file;
            load_request->node_name = PathUtils::get_file(name);
            assert(geo_store->entries.contains(load_request->node_name));
        }
    }
    if(geo_store->loaded)
        return true;

    geo_store->loaded = true;
    if (ctx.prevent_nesting)
        return true;
    // load given prefab's geoset
    GeoSet *gs = geosetLoad(ctx.fs_wrap,geo_store->geopath, m_base_path);
    if(!gs) {

    }
    loadSubgraph(geo_store->geopath,ctx,*this);
    return loadPrefabForNode(getNodeByName(*ctx.m_target,name), ctx);
}

Model *PrefabStore::groupModelFind(const String &path, LoadingContext &ctx)
{
    String model_name = path.substr(path.rfind('/') + 1);
    auto val = groupGetFileEntryPtr(model_name);
    return val ? modelFind(val->geopath, model_name,ctx) : nullptr;
}

GeoStoreDef * PrefabStore::groupGetFileEntryPtr(const String &full_name)
{
    String key = full_name.substr(full_name.rfind('/') + 1);
    key = key.substr(0, key.find("__"));
    return m_modelname_to_geostore.at(key, nullptr);
}

void PrefabStore::sceneGraphWasReset()
{
    for(auto & v : m_dir_to_geoset)
        v.second.loaded = false;
}

PrefabStore::~PrefabStore() {
    for(auto & v : s_name_to_geoset)
        delete v.second;
    s_name_to_geoset.clear();
}

Model *getModelById(GeoSet *gset, int id)
{
    for (Model *v : gset->subs)
    {
        if (id == v->m_id)
            return v;
    }
    return nullptr;
}
