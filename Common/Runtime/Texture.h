/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */
#pragma once
#include "Common/Runtime/HandleBasedStorage.h"
#include "Common/Containers/String.h"
#include "Common/Containers/StringView.h"

#include <glm/vec2.hpp>
#include <stdint.h>

struct ModelModifiers;
struct TextureModifiers;
class QFile;
namespace SEGS
{
struct IFilesystem;
struct GeoSet;

enum class CoHBlendMode : uint8_t
{
    MULTIPLY                = 0,
    MULTIPLY_REG            = 1,
    COLORBLEND_DUAL         = 2,
    ADDGLOW                 = 3,
    ALPHADETAIL             = 4,
    BUMPMAP_MULTIPLY        = 5,
    BUMPMAP_COLORBLEND_DUAL = 6,
    INVALID                 = 255,
};
struct TextureStorage;
struct TextureWrapper
{
    using StorageClass = SEGS::TextureStorage; //tells the handle template to look up
    enum TexFlags
    {
        ALPHA = 0x1,
        RGB8 = 0x2,
        COMP4 = 0x4,
        COMP8 = 0x8,
        DUAL = 0x10,
        TGA = 0x20,
        DDS = 0x40,
        CLAMP_UV = 0x80,
        MOVIE = 0x100,
        CUBEMAPFACE = 0x200,
        REPLACEABLE = 0x400,
        BUMPMAP = 0x800,
        BUMPMAP_MIRROR = 0x1000,
        JPEG = 0x2000,
        CLAMP_U = 0x100040,
        CLAMP_V = 0x100080,
        MIRROR_U = 0x100100,
        MIRROR_V = 0x100200,
        REPEAT_U = 0x100400,
        REPEAT_V = 0x100800,
    };
    String detailname;
    String bumpmap;
    float Gloss;
    int flags {0};
    glm::vec2 scaleUV0 {0,0};
    glm::vec2 scaleUV1 {0,0};
    CoHBlendMode BlendType = CoHBlendMode(0);
    TextureModifiers *info {nullptr};
};
using HTexture = SingularStoreHandleT<20,12,TextureWrapper>;
struct TextureStorage : public HandleBasedStorage<TextureWrapper>
{
    static TextureStorage &instance() 
    {
        static TextureStorage s_instance;
        return s_instance;
    }
};
void loadTexHeader(IFilesystem *fs, StringView fname);
} // end of SEGS namespace
