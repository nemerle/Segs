/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once

#include <glm/vec3.hpp>
#include "Common/Containers/String.h"
#include "Common/Containers/Vector.h"
#include <stdint.h>

struct TailorCost_Data
{
    uint32_t m_MinLevel;
    uint32_t m_MaxLevel;
    uint32_t m_EntryFee;
    uint32_t m_Global;
    uint32_t m_HeadCost;
    uint32_t m_HeadSubCost;
    uint32_t m_UpperCost;
    uint32_t m_UpperSubCost;
    uint32_t m_LowerCost;
    uint32_t m_LoserSubCost;
    uint32_t m_NumCostumes;
};
typedef Vector<TailorCost_Data> AllTailorCosts_Data;

struct ColorEntry_Data
{
    glm::vec3 color;
};

struct Pallette_Data
{
    Vector<ColorEntry_Data> m_Colors;
    String m_Name;
};

struct GeoSet_Mask_Data
{
    String m_Name;
    String m_DisplayName;
    Vector<String> m_Keys;
    String m_Product;
    Vector<String> m_Tag;
    int m_Legacy  = 0;
    int m_DevOnly = 0;
    int m_COV     = 0;
    int m_COH     = 0;
    int m_COHV    = 0;
};

struct GeoSet_Info_Data
{
    String m_DisplayName;
    String m_GeoName;
    String m_Geo;
    String m_Tex1;
    String m_Tex2;
    String m_Fx;
    String m_Product;
    Vector<String> m_Keys;
    Vector<String> m_Tag;
    Vector<String> m_Flags;
    int m_DevOnly;
    int m_COV    = 0;
    int m_COH    = 0;
    int m_COHV   = 0;
    int m_IsMask = 0;
    int m_Level  = 0;
    int m_Legacy = 0;
};

struct CostumeFaceScaleSet
{
    String m_DisplayName;
    glm::vec3 m_Head;
    glm::vec3 m_Brow;
    glm::vec3 m_Cheek;
    glm::vec3 m_Chin;
    glm::vec3 m_Cranium;
    glm::vec3 m_Jaw;
    glm::vec3 m_Nose;
    int m_COV = 0;
    int m_COH = 0;
};

struct GeoSet_Data
{
    String m_Displayname;
    String m_BodyPart;
    String m_ColorLink;
    Vector<String> m_Keys;
    Vector<String> m_Flags;
    String m_Product;
    int m_Type;
    Vector<String> m_AnimBits;
    Vector<String> m_ZoomBits;
    glm::vec3 m_DefaultView;
    glm::vec3 m_ZoomView;
    int m_NumColor  = 0;
    int m_NoDisplay = 0;
    int isOpen;
    int sel_info_idx; // m_Infos index
    int sel_mask_idx; // Mask or MaskString index
    float timing1;
    float timing2;
    Vector<String> m_MaskStrings;
    Vector<String> m_MaskNames;
    Vector<GeoSet_Mask_Data> m_Masks;
    Vector<GeoSet_Info_Data> m_Infos;
    int m_Legacy = 0;
    Vector<CostumeFaceScaleSet> m_Faces;
    int m_COV = 0;
    int m_COH = 0;
};

struct BoneSet_Data
{
    String m_Name;
    String m_FileName;
    String m_Displayname;
    Vector<String> m_Keys;
    String m_Product;
    Vector<String> m_Flags;
    Vector<GeoSet_Data> m_GeoSets;
    int rs=0; // selected geoset index?
    int m_Legacy = 0;
    int m_COV    = 0;
    int m_COH    = 0;
};

struct Region_Data
{
    String m_Name;
    String m_FileName;
    String m_Displayname;
    Vector<String> m_Keys;
    Vector<BoneSet_Data> m_BoneSets;
    String m_StoreCategory;
    int rs=0; // selected boneset
};

struct CostumeSetSet
{
    String m_Name;
    String m_FileName;
    String m_DisplayName;
    String m_NpcName;
    Vector<String> m_Keys;
    Vector<String> m_StoreProductList;
};

struct CostumeGeoData
{
    String m_Name;
    String m_FileName;
    glm::vec3 m_ShieldPos;
    glm::vec3 m_ShieldPYR;
};
struct CostumeOrigin_Data
{
    String m_Name; // Name of the group Male/Female/Huge/BasicFemale ...
    String m_FileName;
    Vector<Pallette_Data> m_BodyPalette;
    Vector<Pallette_Data> m_SkinPalette;
    Vector<Pallette_Data> m_PowerPalette;
    Vector<Region_Data>   m_Region;
    Vector<CostumeSetSet> m_CostumeSets;
    Vector<CostumeGeoData> m_GeoData;
};

struct Costume2_Data
{
    String m_Name;
    Vector<CostumeOrigin_Data> m_Origins;
};

typedef Vector<Costume2_Data> CostumeSet_Data;
struct TaggedCostumePart
{
    String body_part;
    String geometry;
    String tex1;
    String tex2;
    String fx;
    String region;
    String body_set;
};

struct BodyConfig_Data
{
    enum Scale : uint8_t { Global, Bone, Head, Chest, Shoulder, Waist, Hip, Leg, ScaleCount };
    String m_DisplayName;
    String m_BodyType;
    String m_Build;
    float  m_Scales[ScaleCount] = {};
};
typedef Vector<BodyConfig_Data> AllBodyConfigs_Data;
