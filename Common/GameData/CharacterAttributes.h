/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once
#include "Common/Containers/Vector.h"
#include "Common/Containers/String.h"

struct Parse_AttribDesc
{
    String Name;
    String DisplayName;
    String IconName;
    template<class Archive>
    void serialize(Archive & archive);
};

struct AttribNames_Data
{
    Vector<Parse_AttribDesc> m_Damage;
    Vector<Parse_AttribDesc> m_Defense;
    Vector<Parse_AttribDesc> m_Boost;
    Vector<Parse_AttribDesc> m_Group;
    template<class Archive>
    void serialize(Archive & archive);
};

struct Parse_CharAttrib
{
    eastl::array<float,24> m_DamageTypes;
    float m_HitPoints           = 0;
    float m_Endurance           = 0;
    float m_ToHit               = 0;
    eastl::array<float,24> m_DefenseTypes;
    float m_Defense             = 0;
    float m_Evade               = 0;
    float m_SpeedRunning        = 0;
    float m_SpeedFlying         = 0;
    float m_SpeedSwimming       = 0;
    float m_SpeedJumping        = 0;
    float m_jump_height         = 0;
    float m_MovementControl     = 0;
    float m_MovementFriction    = 0;
    float m_Stealth             = 0;
    float m_StealthRadius       = 0;
    float m_PerceptionRadius    = 0;
    float m_Regeneration        = 0;
    float m_Recovery            = 0;
    float m_ThreatLevel         = 0;
    float m_Taunt               = 0;
    float m_Confused            = 0;
    float m_Afraid              = 0;
    float m_Held                = 0;
    float m_Immobilized         = 0;
    float m_is_stunned          = 0;
    float m_Sleep               = 0;
    float m_is_flying           = 0;
    float m_has_jumppack        = 0;
    float m_Teleport            = 0;
    float m_Untouchable         = 0;
    float m_Intangible          = 0;
    float m_OnlyAffectsSelf     = 0;
    float m_Knockup             = 0;
    float m_Knockback           = 0;
    float m_Repel               = 0;
    float m_Accuracy            = 0;
    float m_Radius              = 0;
    float m_Arc                 = 0;
    float m_Range               = 0;
    float m_TimeToActivate      = 0;
    float m_RechargeTime        = 0;
    float m_InterruptTime       = 0;
    float m_EnduranceDiscount   = 0;
    float *begin() { return &m_DamageTypes[0]; }
    float *end() { return (&m_EnduranceDiscount)+1; }
    const float *begin() const { return &m_DamageTypes[0]; }
    const float *end() const { return (&m_EnduranceDiscount)+1; }
    void initAttribArrays()
    {
        m_DamageTypes.fill(0.0f);
        m_DefenseTypes.fill(0.0f);
    }
    template<class Archive>
    void serialize(Archive & archive);
};

struct Parse_CharAttribMax
{
    eastl::array<Vector<float>,24> m_DamageTypes ;
    Vector<float> m_HitPoints;
    Vector<float> m_Endurance;
    Vector<float> m_ToHit;
    eastl::array<Vector<float>,24> m_DefenseTypes;
    Vector<float> m_Defense;
    Vector<float> m_Evade;
    Vector<float> m_SpeedRunning;
    Vector<float> m_SpeedFlying;
    Vector<float> m_SpeedSwimming;
    Vector<float> m_SpeedJumping;
    Vector<float> m_jump_height;
    Vector<float> m_MovementControl;
    Vector<float> m_MovementFriction;
    Vector<float> m_Stealth;
    Vector<float> m_StealthRadius;
    Vector<float> m_PerceptionRadius;
    Vector<float> m_Regeneration;
    Vector<float> m_Recovery;
    Vector<float> m_ThreatLevel;
    Vector<float> m_Taunt;
    Vector<float> m_Confused;
    Vector<float> m_Afraid;
    Vector<float> m_Held;
    Vector<float> m_Immobilized;
    Vector<float> m_is_stunned;
    Vector<float> m_Sleep;
    Vector<float> m_is_flying;
    Vector<float> m_has_jumppack;
    Vector<float> m_Teleport;
    Vector<float> m_Untouchable;
    Vector<float> m_Intangible;
    Vector<float> m_OnlyAffectsSelf;
    Vector<float> m_Knockup;
    Vector<float> m_Knockback;
    Vector<float> m_Repel;
    Vector<float> m_Accuracy;
    Vector<float> m_Radius;
    Vector<float> m_Arc;
    Vector<float> m_Range;
    Vector<float> m_TimeToActivate;
    Vector<float> m_RechargeTime;
    Vector<float> m_InterruptTime;
    Vector<float> m_EnduranceDiscount;
    template<class Archive>
    void serialize(Archive & archive);
};
