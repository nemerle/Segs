#include "RuntimeData.h"

#include "Prefab.h"
#include "Texture.h"
#include "Components/Logging.h"
#include "Common/GameData/trick_definitions.h"
#include "Common/GameData/trick_serializers.h"
#include "Common/GameData/DataStorage.h"
#include "Components/serialization_common.h"


using namespace SEGS;
namespace {
static void setupTexOpt(SceneModifiers *mods,TextureModifiers *tmod)
{
    if(tmod->ScaleST0.x == 0.0f)
        tmod->ScaleST0.x = 1.0f;
    if(tmod->ScaleST0.y == 0.0f)
        tmod->ScaleST0.y = 1.0f;
    if(tmod->ScaleST1.x == 0.0f)
        tmod->ScaleST1.x = 1.0f;
    if(tmod->ScaleST1.y == 0.0f)
        tmod->ScaleST1.y = 1.0f;
    if(tmod->Fade.x != 0.0f || tmod->Fade.y != 0.0f)
        tmod->Flags |= uint32_t(TexOpt::FADE);
    if(!tmod->Blend.empty())
        tmod->Flags |= uint32_t(TexOpt::DUAL);
    if(!tmod->Surface.empty())
    {
        //sCDebug(logSceneGraph) << "Has surface" << tex->Surface;
    }

    tmod->name = tmod->name.substr(0,tmod->name.rfind('.')); // cut last extension part
    if(tmod->name.starts_with('/'))
        tmod->name.erase(0,1);
    if(tmod->name.ends_with('/'))
        tmod->name.pop_back();

    String lower_name = tmod->name.to_lower();
    auto iter = mods->m_texture_path_to_mod.find(lower_name);
    if(iter!=mods->m_texture_path_to_mod.end())
    {
        sCDebug(logSceneGraph) << "Duplicate texture info: " << tmod->name;
        return;
    }
    mods->m_texture_path_to_mod[lower_name] = tmod;
}
static void setupTrick(SceneModifiers *mods,GeometryModifiers *gmod)
{
    if(gmod->node.TintColor0.rgb_are_zero())
        gmod->node.TintColor0 = RGBA(0xFFFFFFFF);
    if(gmod->node.TintColor1.rgb_are_zero())
        gmod->node.TintColor1 = RGBA(0xFFFFFFFF);
    gmod->AlphaRef /= 255.0f;
    if(gmod->ObjTexBias != 0.0f)
        gmod->node._TrickFlags |= TexBias;
    if(gmod->AlphaRef != 0.0f)
        gmod->node._TrickFlags |= AlphaRef;
    if(gmod->FogDist.x != 0.0f || gmod->FogDist.y != 0.0f)
        gmod->node._TrickFlags |= FogHasStartAndEnd;
    if(gmod->ShadowDist != 0.0f)
        gmod->node._TrickFlags |= CastShadow;
    if(gmod->NightGlow.x != 0.0f || gmod->NightGlow.y != 0.0f)
        gmod->node._TrickFlags |= NightGlow;
    if(gmod->node.ScrollST0.x != 0.0f || gmod->node.ScrollST0.y != 0.0f)
        gmod->node._TrickFlags |= ScrollST0;
    if(gmod->node.ScrollST1.x != 0.0f || gmod->node.ScrollST1.y != 0.0f)
        gmod->node._TrickFlags |= ScrollST1;
    if(!gmod->StAnim.empty())
    {
        //        if(setStAnim(&a1->StAnim.front()))
        //            a1->node._TrickFlags |= STAnimate;
    }
    if(gmod->GroupFlags & VisTray)
        gmod->ObjFlags |= 0x400;
    if(gmod->name.empty())
        sCDebug(logSceneGraph) << "No name in trick";
    String lower_name = gmod->name.to_lower();
    auto iter = mods->g_tricks_string_hash_tab.find(lower_name);
    if(iter!=mods->g_tricks_string_hash_tab.end())
    {
        sCDebug(logSceneGraph) << "duplicate model trick!";
        return;
    }
    mods->g_tricks_string_hash_tab[lower_name]=gmod;
}

static void trickLoadPostProcess(SceneModifiers *mods)
{
    mods->m_texture_path_to_mod.clear();
    mods->g_tricks_string_hash_tab.clear();
    for(TextureModifiers &texopt : mods->texture_mods)
        setupTexOpt(mods,&texopt);
    for(GeometryModifiers &trickinfo : mods->geometry_mods)
        setupTrick(mods,&trickinfo);
}

template<class TARGET,unsigned int CRC>
bool read_data_to(IFilesystem *fs, const String &directory_path, const String &storage, TARGET &target)
{
    auto deb = sDebug(); //.noquote().nospace()
    deb << "Reading " << directory_path << storage << " ... ";
    BinStore bin_store;
    if(!bin_store.open(directory_path+storage,CRC))
    {
        deb << "failure";
        sWarning() << "Couldn't load" << storage << "from" << directory_path;
        sWarning() << "Using piggtool, ensure that bin.pigg has been extracted to ./data/";
        return false;
    }

    bool res=loadFrom(&bin_store,target);
    if(res)
        deb << "OK";
    else
    {
        deb << "failure";
        sWarning() << "Couldn't load" << directory_path<<storage<<": wrong file format?";
    }

    return res;
}

} // end of anonymous namespace


namespace SEGS
{

void preloadTextureNames(IFilesystem *fs,const String &basepath)
{
    RuntimeData &rd(getRuntimeData());
    String textures_path = basepath + "texture_library";
    int tex_count=0;
    //TODO: store texture headers into an array, and only rescan directories when forced ?
    fs->visitEntries(textures_path,
        [&](StringView fpath,bool is_dir)->SEGS::IFilesystem::VisitResult {
            if(is_dir) {
                return SEGS::IFilesystem::VisitSubdirectory;
    }
            StringView path_str = fpath;
            if(!path_str.ends_with(".texture")) {
                return SEGS::IFilesystem::VisitNext;
            }
            String texture_key = String(PathUtils::get_basename(PathUtils::get_file(path_str))).to_lower();
            rd.m_texture_paths[texture_key] = path_str;
            loadTexHeader(fs,fpath);
            return SEGS::IFilesystem::VisitNext;
        });
    sInfo()<<"Loaded " << StringUtils::num_int64(tex_count);
}

} //end of SEGS namespace

bool RuntimeData::read_model_modifiers(const String &directory_path)
{
    if(m_modifiers)
        return true;
    SceneModifiers tricks_store;
    assert(m_wrapper);
    if(!read_data_to<SceneModifiers,tricks_i0_requiredCrc>(m_wrapper,directory_path,"bin/tricks.bin", tricks_store))
    {
        return false;
    }
    m_modifiers = new SceneModifiers;
    *m_modifiers = eastl::move(tricks_store);
    trickLoadPostProcess(m_modifiers);
    return true;
}

RuntimeData::RuntimeData()
{
}

RuntimeData::~RuntimeData()
{
}

bool RuntimeData::prepare(IFilesystem* fs, const String &directory_path)
{
    m_wrapper = fs;
    m_ready = false;
    if(!read_prefab_definitions(directory_path))
        return false;
    if(!read_model_modifiers(directory_path))
        return false;

    m_ready = true;
    return true;
}

bool RuntimeData::read_prefab_definitions(const String &directory_path)
{
    if(!m_prefab_mapping)
        m_prefab_mapping = new SEGS::PrefabStore(m_wrapper,directory_path);
    return m_prefab_mapping->prepareGeoLookupArray(directory_path);
}

using namespace SEGS;
RuntimeData &getRuntimeData()
{
    static RuntimeData instance;
    return instance;
}

void destroyRuntimeData() {
    RuntimeData &rd(getRuntimeData());
    delete rd.m_prefab_mapping;
    delete rd.m_modifiers;
    rd.m_loaded_textures.clear();
    rd.m_prefab_mapping = nullptr;
    rd.m_modifiers = nullptr;
}
