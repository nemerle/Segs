#include "Texture.h"

#include "Common/Containers/Set.h"
#include "RuntimeData.h"
#include "Components/Logging.h"
#include "Components/serialization_common.h"
#include "Common/GameData/GameDataStore.h"
#include "Common/GameData/trick_definitions.h"

using namespace SEGS;

namespace
{
#pragma pack(push, 1)
struct TexFileHdr
{
    int     header_size;
    int     file_size;
    int     wdth;
    int     hght;
    int     flags;
    int     fade[2];
    uint8_t alpha;
    char    magic[3];
};
#pragma pack(pop)

Set<String> s_missing_textures;
///
/// \brief Will split the \arg texpath into directories, and finds the closest TextureModifiers
/// that matches a directory
/// \param texpath contains a full path to the texture
/// \return texture modifier object, if any
///
TextureModifiers *modFromTextureName(StringView texpath)
{
    RuntimeData &rd(getRuntimeData());
    Vector<StringView> split = StringUtils::split(texpath,"/");
    while(!split.empty())
    {
        if(0==StringUtils::compare(split.front(),"texture_library")) {
            split.pop_front();
            break;
        }
        split.pop_front();
    }
    SceneModifiers *mods = rd.m_modifiers;
    assert(mods);
    const HashMap<String,TextureModifiers *> &texmods(mods->m_texture_path_to_mod);
    // scan from the back of the texture path, until a modifier is found.
    while(!split.empty())
    {
        auto lookup_str=StringUtils::to_lower(split.back());
        auto val = texmods.at(lookup_str,nullptr);
        if(val)
        {
            //qDebug() << "located tex mod" << String::joined(split,"/");
            return val;
        }
        split.pop_back();
    }
    return nullptr;
}
}

namespace SEGS
{

void loadTexHeader(IFilesystem *fs,StringView fname)
{
    RuntimeData &rd(getRuntimeData());
    TextureWrapper res;
    StringView tex_path(fname);
    String lookupstring=String(PathUtils::get_basename(tex_path)).to_lower();
    const String &actualPath(rd.m_texture_paths[lookupstring]);
    if(actualPath.empty())
    {
        if(!s_missing_textures.contains(lookupstring))
        {
            sCDebug(logSceneGraph) << "Missing texture" << fname;
            s_missing_textures.insert(lookupstring);
        }
        return;
    }
    auto src_tex = fs->openFile(actualPath,IFile::ReadOnly);
    if(src_tex)
    {
        TexFileHdr hdr;
        src_tex->read((char *)&hdr, sizeof(TexFileHdr));
        if(0 == memcmp(hdr.magic, "TX2", 3))
        {
            if(hdr.alpha) {
                res.flags |= TextureWrapper::ALPHA;
            }
            if ( hdr.alpha && !(hdr.flags & TexHeaderOpt::FADE) )
                res.flags |= TextureWrapper::ALPHA;
            if(hdr.flags & TexHeaderOpt::BUMPMAP)
                res.flags |= TextureWrapper::BUMPMAP;
        }
    }
    StringView actualPathView(PathUtils::path(actualPath));
    auto loc = actualPath.rfind('.');
    StringView texNameForMods(loc!=String::npos ? StringView(actualPath).substr(0,loc) : actualPathView);

    //qDebug() << "Loading texture" << texNameForMods;

    res.info = modFromTextureName(texNameForMods);
    uint32_t texopt_flags = 0;
    if(res.info)
        texopt_flags = res.info->Flags;
    String upper_fname(String(fname).to_upper());
    if(upper_fname.contains("PLAYERS/") || upper_fname.contains("ENEMIES/") || upper_fname.contains("NPCS/"))
        res.flags |= TextureWrapper::BUMPMAP_MIRROR | TextureWrapper::CLAMP_UV;

    if(upper_fname.contains("MAPS/"))
        res.flags |= TextureWrapper::CLAMP_UV;

    if(texopt_flags & REPLACEABLE)
        res.flags |= TextureWrapper::REPLACEABLE;

    if(texopt_flags & BUMPMAP)
        res.flags |= TextureWrapper::BUMPMAP;
    if(texopt_flags & CLAMP_U) {
        res.flags |= TextureWrapper::CLAMP_U;
    }

    if(texopt_flags & CLAMP_V) {
        res.flags |= TextureWrapper::CLAMP_V;
    }

    if(texopt_flags & MIRROR_U) {
        res.flags |= TextureWrapper::MIRROR_U;
    }
    if(texopt_flags & MIRROR_V) {
        res.flags |= TextureWrapper::MIRROR_V;
    }

    if(texopt_flags & REPEAT_U) {
        res.flags |= TextureWrapper::REPEAT_U;
    }
    if(texopt_flags & REPEAT_V) {
        res.flags |= TextureWrapper::REPEAT_V;
    }

    res.scaleUV0 = {1,1};
    res.scaleUV1 = {1,1};
    res.Gloss= res.info ? res.info->Gloss : 1.0f;

    if(res.info && !res.info->BumpMap.empty())
        res.bumpmap = res.info->BumpMap;
    String detailname;
    if(texopt_flags & DUAL)
    {
        if(!res.info->Blend.empty())
        {
            res.flags |= TextureWrapper::DUAL;
            res.BlendType = CoHBlendMode(res.info->BlendType);
            res.scaleUV0 = {res.info->ScaleST0.x,res.info->ScaleST0.y};
            res.scaleUV1 = {res.info->ScaleST1.x,res.info->ScaleST1.y};
            res.detailname = res.info->Blend;

            if(res.BlendType == CoHBlendMode::ADDGLOW && 0==StringUtils::compare(res.detailname,"grey",StringUtils::CaseInsensitive))
            {
                res.detailname = "black";
            }
            // copy the 'res' into the handle based storage, and record the handle
            rd.m_loaded_textures[lookupstring] = TextureStorage::instance().create(res);
            return;
        }
        sCDebug(logSceneGraph) << "Detail texture " << res.info->Blend << " does not exist for texture mod" << res.info->name;
        detailname = "grey";
    }
    else if(lookupstring.compare("invisible")==0)
    {
        detailname = "invisible";
    }
    else
    {
        detailname = "grey";
    }
    if(res.BlendType == CoHBlendMode::ADDGLOW && 0==StringUtils::compare(detailname,"grey",StringUtils::CaseInsensitive))
    {
        detailname = "black";
    }
    res.detailname = detailname;
    // copy the 'res' into the handle based storage, and record the handle
    rd.m_loaded_textures[lookupstring] = TextureStorage::instance().create(res);
}
}
