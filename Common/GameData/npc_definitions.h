/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once

#include "Components/Colors.h"

#include "Common/Containers/String.h"
#include "Common/Containers/Vector.h"

struct Parse_CostumePart
{
    String m_Name;
    RGBA m_Color1;
    RGBA m_Color2;
    String m_Texture1;
    String m_Texture2;
    String m_CP_Geometry;
};

enum class BodyType : int
{
    Male        = 0,
    Female      = 1,
    BasicMale   = 2,
    BasicFemale = 3,
    Huge        = 4,
    Enemy       = 5,
    Villain     = 6,
};

struct Parse_Costume
{
    String  m_EntTypeFile;
    String  m_CostumeFilePrefix;
    BodyType m_BodyType  = BodyType::Male;
    float    m_Scale     = 0;
    float    m_BoneScale = 0;
    RGBA     m_SkinColor;
    uint32_t m_NumParts = 0;
    Vector<Parse_CostumePart> m_CostumeParts;
};

struct NPCPower_Desc
{
    String PowerCategory;
    String PowerSet;
    String Power;
    int Level;
    int Remove;
};

struct Parse_NPC
{
    String m_Name;
    String m_DisplayName;
    int m_Rank;
    String m_Class;
    int m_Level;
    int m_XP;
    Vector<NPCPower_Desc> m_Powers;
    Vector<Parse_Costume> m_Costumes;
    bool has_variant(uint32_t idx) const { return idx<m_Costumes.size(); }
};
using AllNpcs_Data = Vector<Parse_NPC>;

BodyType bodyTypeForEntType(const String &enttypename);
String entTypeFileName(const Parse_Costume *costume);
String bodytype_prefix_fixup(const Parse_Costume *a1, const String &a2);
