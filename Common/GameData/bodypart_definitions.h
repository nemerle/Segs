/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once
#include "Common/Containers/String.h"
#include "Common/Containers/Vector.h"

enum BoneIds
{
    MAX_BONES=70
};

struct BodyPart_Data
{
    String m_Name;
    String m_GeoName;
    String m_TexName;
    String m_BaseName;
    int m_BoneCount;
    int m_InfluenceCost;
    // Transient data.
    eastl::array<int,2> boneIndices;
    int part_idx;
};
struct BodyPartsStorage
{
    Vector<BodyPart_Data> m_parts;

    BodyPart_Data *getBodyPartFromName(const String &name);

    void postProcess();
};

namespace SEGS
{
bool legitBone(int idx);
const char *boneName(int idx);
}
