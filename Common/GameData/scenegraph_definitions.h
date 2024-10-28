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

struct GroupLoc_Data
{
    String name;
    glm::vec3 pos {0,0,0};
    glm::vec3 rot {0,0,0};
};

struct GroupProperty_Data
{
    String propName;
    String propValue;
    int propertyType; // 1 - propValue contains float radius, 0 propValue is plain string
};

struct TintColor_Data
{
    uint32_t clr1=0;
    uint32_t clr2=0;
};

struct ReplaceTex_Data
{
    int texIdxToReplace=0;
    String repl_with;
};

struct DefSound_Data
{
    String name;
    float volRel1;
    float sndRadius;
    float snd_ramp_feet;
    uint32_t sndFlags;
};

struct DefLod_Data
{
    float Far;
    float FarFade;
    float Near;
    float NearFade;
    float Scale;
};

struct DefOmni_Data
{
    uint32_t omniColor;
    float Size;
    int isNegative;
};

struct DefBeacon_Data
{
    String name;
    float amplitude; // maybe rotation speed ?
};

struct DefFog_Data
{
    float fogZ;
    float fogX;
    float fogY;
    uint32_t fogClr1;
    uint32_t fogClr2;
};

struct DefAmbient_Data
{
    uint32_t clr;
};

struct SceneGraphNode_Data
{
    enum
    {
        Ungroupable = 1,
        FadeNode = 2,
    };
    String name;
    String p_Obj;
    String type;
    int flags;
    Vector<GroupLoc_Data> p_Grp;
    Vector<GroupProperty_Data> p_Property;
    Vector<TintColor_Data> p_TintColor;
    Vector<DefSound_Data> p_Sound;
    Vector<ReplaceTex_Data> p_ReplaceTex;
    Vector<DefOmni_Data> p_Omni;
    Vector<DefBeacon_Data> p_Beacon;
    Vector<DefFog_Data> p_Fog;
    Vector<DefAmbient_Data> p_Ambient;
    Vector<DefLod_Data> p_Lod;
};

struct SceneRootNode_Data
{
    String name;
    glm::vec3 pos {0,0,0};
    glm::vec3 rot {0,0,0};
};

struct SceneGraph_Data
{
    Vector<SceneGraphNode_Data> Def;
    Vector<SceneRootNode_Data> Ref;
    String Scenefile;
    int Version;
};
