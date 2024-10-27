#pragma once
#include "Common/Runtime/HandleBasedStorage.h"

#include "Common/Runtime/Texture.h"
#include "Common/Containers/HashMap.h"
#include "Common/Containers/String.h"

struct SceneModifiers;
namespace SEGS
{
    struct IFilesystem;
    struct RuntimeData;
}
extern SEGS::RuntimeData& getRuntimeData();
extern void destroyRuntimeData();
namespace SEGS
{
using HTexture = SingularStoreHandleT<20,12,struct TextureWrapper>;
struct PrefabStore;

struct RuntimeData
{
    //! Here we store handles to loaded texture headers
    HashMap<String, HTexture> m_loaded_textures;
    //! map from texture name to full file path
    HashMap<String, String> m_texture_paths;
    PrefabStore *           m_prefab_mapping = nullptr; //!< maps directories and model names to geosets
    SceneModifiers *        m_modifiers      = nullptr;
    IFilesystem *           m_wrapper        = nullptr;
    bool                    m_ready          = false; //!< set to true if runtime data was read.
    bool prepare(IFilesystem *fs,const String &directory_path);


    bool read_prefab_definitions(const String &directory_path);
    bool read_model_modifiers(const String &directory_path);
    // This is a non-copyable type
    RuntimeData(const RuntimeData &) = delete;
    RuntimeData &operator=(const RuntimeData&) = delete;
private:
    friend RuntimeData& ::getRuntimeData();
    RuntimeData();
    ~RuntimeData();
};
void preloadTextureNames(IFilesystem *fs,const String &basepath);
} //end of SEGS namespace

