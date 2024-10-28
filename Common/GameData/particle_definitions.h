/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once

#include "Common/GameData/fx_definitions.h"
#include <glm/vec2.hpp>

struct Texture;
struct ParticleTextureData
{
    String    m_TextureName;
    glm::vec3 m_TexScroll;
    glm::vec3 m_TexScrollJitter;
    float     m_AnimFrames;
    float     m_AnimPace;
    uint32_t  m_AnimType;
};

struct ParticleSystemInfo
{
    uint32_t               m_WorldOrLocalPosition;
    uint32_t               m_FrontOrLocalFacing;
    float                  m_TimeToFull;
    uint32_t               m_KickStart;
    Vector<float>          m_NewPerFrame;
    Vector<uint32_t>       m_Burst;
    float                  m_MoveScale;
    uint32_t               m_BurbleType;
    float                  m_BurbleFrequency;
    Vector<float>          m_BurbleAmplitude;
    float                  m_BurbleThreshold;
    uint32_t               m_EmissionType;
    Vector<glm::vec3>      m_EmissionStartJitter;
    float                  m_EmissionRadius;
    float                  m_EmissionHeight;
    int                    m_Spin;
    uint32_t               m_SpinJitter;
    uint32_t               m_OrientationJitter;
    float                  m_Magnetism;
    float                  m_Gravity;
    uint32_t               m_KillOnZero;
    uint32_t               m_Terrain;
    Vector<glm::vec3>      m_InitialVelocity;
    Vector<glm::vec3>      m_InitialVelocityJitter;
    glm::vec3              m_VelocityJitter;
    float                  m_TightenUp;
    float                  m_SortBias;
    float                  m_Drag;
    float                  m_Stickiness;
    Vector<uint32_t>  m_Alpha;
    uint32_t               m_ColorChangeType;
    eastl::array<ColorFx, 5> m_StartColor; // colornavpoint
    float                  m_FadeInBy;
    float                  m_FadeOutStart;
    float                  m_FadeOutBy;
    uint32_t               m_Blend_mode;
    Vector<float>          m_StartSize;
    float                  m_StartSizeJitter;
    Vector<float>          m_EndSize;
    float                  m_ExpandRate;
    enum
    {
        // Expansion is based on timestep * m_ExpandRate
        Expand_Unbound  = 0, // Unbound expansion
        Expand_Bound    = 1, // grow until size reaches m_EndSize
        Expand_PingPong = 2, // grow from m_StartSize to m_EndSize, and than back to m_StartSize
    };
    uint32_t                          m_ExpandType;
    uint32_t                          m_StreakType;
    uint32_t                          m_StreakOrient;
    uint32_t                          m_StreakDirection;
    float                             m_StreakScale;
    eastl::array<ParticleTextureData, 2> particleTexture;
    String                            m_Name;
    String                            m_DieLikeThis;
    uint32_t                          m_DeathAgeToZero;
    uint32_t                          m_Flags;
    float                             m_VisRadius;
    float                             m_VisDist;
};
struct Parse_AllPSystems
{
    Vector<ParticleSystemInfo> m_Systems;
    Map<String,size_t> m_NameToIdx;
};
void cleanupPSystemName(String &name);
