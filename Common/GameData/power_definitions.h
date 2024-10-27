/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once
#include "Common/Containers/HashMap.h"
#include <Common/Containers/Vector.h>
#include <Common/Containers/String.h>

namespace SEGS_Enums
{
enum class SeqBitNames : uint32_t;
}
using namespace SEGS_Enums;

namespace SEGS_Enums_Power
{

enum class StoredPower_Type
{
    Click       = 0,
    Auto        = 1,
    Toggle      = 2,
    Boost       = 3,
    Inspiration = 4,
};

enum class StoredEntEnum : uint32_t
{
    None                = 0,
    Caster              = 1,
    Player              = 2,
    DeadPlayer          = 3,
    Teammate            = 4,
    Enemy               = 5,
    DeadVillain         = 6,
    NPC                 = 7,
    Friend              = 8,
    Foe                 = 9,
    Location            = 10,
    Any                 = 11,
    DeadTeammate        = 12,
    DeadOrAliveTeammate = 13,
    Teleport            = 14,
};

enum StoredVisibility : uint32_t
{
    LineOfSight    = 0,
    VisibilityNone = 1,
};

enum class StoredAffectArea : uint32_t
{
    Character = 0,
    Cone      = 1,
    Sphere    = 2,
    Location  = 3,
};

enum class StoredAiReport : uint32_t
{
    Always   = 0,
    Never    = 1,
    HitsOnly = 2,
    MissOnly = 3,
};

enum DurationEnum : int32_t
{
    kInstant     = -1,
    kUntilKilled = 999999, // Until Shut Off
};

enum class AttribStackType
{
    Stack   = 0,
    Ignore  = 1,
    Extend  = 2,
    Replace = 3
};

enum class AttribModType : uint32_t
{
    Duration  = 0,
    Magnitude = 1,
    Constant  = 2,
};

enum class AttribModTarget : uint32_t
{
    Self   = 0,
    Target = 1,
};

enum class AttribMod_Aspect : uint32_t
{
    Current         = 0,
    Maximum         = 4,
    Strength        = 8,
    Resistance      = 0xC,
    Absolute        = 0x10,
    CurrentAbsolute = 0x10,
};

enum class PowerType : uint32_t
{
    Click       = 0,
    Auto        = 1,
    Toggle      = 2,
    Boost       = 3,
    Inspiration = 4,
    NumTypes    = 5,
};

// I'm unsure as to the validity of this
enum class AttackType : uint32_t {
    None = 0,
    Claw = 1,
    Kunfu = 2,
    Gun = 3,
    Blade = 4,
    Blunt = 5,
    Handgun = 6,
    Combat = 7,
    Weapon = 8,
    Speed = 9,
    Teleport = 10,
    Carry = 11,
    Club = 12,
    Ranged = 108,
    Melee = 112,
    Aoe = 116,
    Smashing = 120,
    Lethal = 124,
    Fire = 128,
    Cold = 132,
    Energy = 136,
    Negative_Energy = 140
};

} // end of SEGS_Enums_Power namespace
using namespace SEGS_Enums_Power;

struct StoredAttribMod
{
public:
    String         name             = "unknown";
    int                index_in_power;
    String         DisplayAttackerHit = "";
    String         DisplayVictimHit = "";
    struct Power_Data *parent_StoredPower;
    AttribModTarget    Target           = AttribModTarget::Target;
    String         Table            = "43"; //would be easier as int, since the tables are stored in an array
    float              Scale            = 1.0;
    int                Attrib           = 0; //
    AttribMod_Aspect   Aspect;
    AttribModType      Type             = AttribModType::Magnitude;
    float              Duration         = 0.0; // Special values in DurationEnum
    float              Magnitude        = 1.0;
    int                Delay            = 0;    //should be float, for now I'm just dividing by 1000
    int                Period           = 0;    //same
    int                Chance           = 100;  // are we sure this isn't a float?
    int                CancelOnMiss;            // bool?
    int                NearGround;              // bool?
    int                AllowStrength;           // bool?
    int                AllowResistance;         // bool?
    AttribStackType    StackType     = AttribStackType::Replace;
    Vector<int>   ContinuingBits;
    String         ContinuingFX;
    Vector<int>   ConditionalBits; // 5c
    String         ConditionalFX;
    String         EntityDef;
    String         PriorityListOffense;
    String         PriorityListDefense;
    String         PriorityListPassive;
};

struct Power_Data
{
    String                   m_Name;
    int                          ptr_powerset_available;
    struct Parse_PowerSet *      parent_StoredPowerSet;
    int                          category_idx;
    int                          powerset_idx;
    int                          power_index;
    String                   DisplayName;
    String                   DisplayHelp;
    String                   DisplayShortHelp;
    String                   DisplayAttackerAttack;
    String                   DisplayAttackerHit;
    String                   DisplayVictimHit;
    String                   IconName;
    Vector<SeqBitNames>     ModeSeqBits;
    Vector<SeqBitNames>     ActivationBits;
    Vector<SeqBitNames>     WindUpBits; // 3c
    Vector<SeqBitNames>     InitialAttackBits;
    Vector<SeqBitNames>     AttackBits;
    Vector<SeqBitNames>     HitBits;
    Vector<SeqBitNames>     BlockBits;
    Vector<SeqBitNames>     DeathBits;
    String                   ActivationFX;
    String                   WindUpFX;
    String                   InitialAttackFX;
    String                   AttackFX;
    String                   BlockFX;
    String                   HitFX;
    String                   DeathFX;
    int                          m_InitialFramesBeforeHit;
    int                          m_FramesBeforeHit;
    int                          m_AttackFrames;
    int                          DelayedHit;
    int                          ProjectileSpeed;
    PowerType                    Type;
    Vector<AttackType>      AttackTypes;
    Vector<String>      Requires;
    float                        Accuracy;
    int                          IgnoreStrength;
    int                          NearGround;
    int                          TargetNearGround;
    int                          CastableAfterDeath;
    int                          AIReport;
    StoredAffectArea             EffectArea;
    float                        Radius;
    float                        Arc;
    float                        Range;
    float                        RangeSecondary;
    float                        InitialFramesBeforeHit_seconds;
    float                        FramesBeforeHit_seconds;
    float                        TimeToActivate;
    float                        RechargeTime;
    float                        InterruptTime;
    float                        ActivatePeriod;
    float                        EnduranceCost;
    int                          DestroyOnLimit;
    int                          limited_use;
    int                          m_NumCharges;
    float                        m_UsageTime;
    int                          has_lifetime;
    float                        m_Lifetime;
    StoredVisibility             TargetVisibility;
    StoredEntEnum                Target;
    StoredEntEnum                TargetSecondary;
    Vector<StoredEntEnum>   EntsAffected;
    Vector<StoredEntEnum>   EntsAutoHit;
    Vector<uint32_t>        BoostsAllowed;
    Vector<uint32_t>        GroupMembership;
    Vector<String>      AIGroups;
    Vector<StoredAttribMod> pAttribMod;
    int                          fDamageGiven;
    int                          iCntUsed;
    int                          iCntHits;
    int                          iCntMisses;
};

struct Parse_PowerSet
{
    String                   m_Name;
    struct StoredPowerCategory * parent_PowerCategory;
    String                   DisplayName;
    String                   DisplayHelp;
    String                   DisplayShortHelp;
    String                   IconName;
    Vector<Power_Data>      m_Powers;
    HashMap<String, Power_Data *> m_hash_table;
    Vector<int32_t>         Available;
};

enum
{
    kCategory_Count = 3
};

struct StoredPowerCategory
{
    String                       name;
    String                       disp_name;
    String                       disp_help;
    String                       disp_short_help;
    Vector<Parse_PowerSet>      m_PowerSets;
    HashMap<String, Parse_PowerSet *> m_powers_hash;
};

struct AllPowerCategories
{
    Vector<StoredPowerCategory>      m_categories;
    HashMap<String, StoredPowerCategory *> categories_hash;
};
