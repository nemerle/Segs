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
};

struct GeoSet_Mask_Data
{
    String m_Name;
    String m_DisplayName;
};

struct GeoSet_Info_Data
{
    String m_DisplayName;
    String m_GeoName;
    String m_Geo;
    String m_Tex1;
    String m_Tex2;
    int m_DevOnly;
};

struct GeoSet_Data
{
    String m_Displayname;
    String m_BodyPart;
    int m_Type;
    int isOpen;
    int sel_info_idx; // m_Infos index
    int sel_mask_idx; // Mask or MaskString index
    float timing1;
    float timing2;
    Vector<String> m_MaskStrings;
    Vector<String> m_MaskNames;
    Vector<GeoSet_Mask_Data> m_Masks;
    Vector<GeoSet_Info_Data> m_Infos;
};

struct BoneSet_Data
{
    String m_Name;
    String m_Displayname;
    Vector<GeoSet_Data> m_GeoSets;
    int rs=0; // selected geoset index?
};

struct Region_Data
{
    String m_Name;
    String m_Displayname;
    Vector<BoneSet_Data> m_BoneSets;
    int rs=0; // selected boneset
};

struct CostumeOrigin_Data
{
    String m_Name; // Name of the group Male/Female/Huge/BasicFemale ...
    Vector<Pallette_Data> m_BodyPalette;
    Vector<Pallette_Data> m_SkinPalette;
    Vector<Region_Data>   m_Region;
};

struct Costume2_Data
{
    String m_Name;
    Vector<CostumeOrigin_Data> m_Origins;
};

typedef Vector<Costume2_Data> CostumeSet_Data;
