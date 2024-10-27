#pragma once
#include "Components/Colors.h"

#include "Common/GameData/anim_definitions.h"
#include "Common/GameData/seq_definitions.h"

#include <Common/Containers/Vector.h>
#include <EASTL/array.h>

struct TextureAnim_Data;

struct ColorFx
{
    RGBA startcolor;
    int startTime;
};
enum SplatFlags {
    SPLAT_ADDITIVE = 1,
    SPLAT_SHADOWFADE = 0x2,
    SPLAT_ADDBASE  = 4,
};
enum SplatFalloffType : uint32_t;
struct FxBehavior
{
    String  name;
    bool initialized=false;
    int lastChangeDate=0;
    glm::vec3 m_StartJitter;
    glm::vec3 m_InitialVelocity;
    glm::vec3 m_InitialVelocityJitter;
    float m_Gravity=0;
    glm::vec3 m_Spin;
    glm::vec3 m_SpinJitter;
    float m_FadeInLength=0;
    float m_FadeOutLength=0;
    uint8_t m_Alpha=0;
    eastl::array<ColorFx,5> m_ColorEffect;
    Vector<RGB> m_preparedColors;
    glm::vec3 m_Scale;
    glm::vec3 m_ScaleRate;
    glm::vec3 m_ScaleTime;
    glm::vec3 m_EndScale;
    uint8_t m_Stretch=0;
    uint8_t m_Collides=0;
    glm::vec3 m_PyrRotate;
    glm::vec3 m_PositionOffset;
    float m_TrackRate=0;
    int m_TrackMethod=0;
    int m_LifeSpan=0;
    float m_AnimScale=0;
    float m_Shake=0;
    float m_ShakeFallOff=0;
    float m_ShakeRadius=0;
    int m_SplatFlags=0;
    Vector<TextureAnim_Data> m_stAnim;
    SplatFalloffType m_SplatFalloffType=SplatFalloffType(0);
    float m_SplatNormalFade=0;
    float m_SplatSetBack=0;
    int m_SplatFadeCenter=0;
    float m_FadeInRate=0;
    float fadeoutrate=0;
    float m_PulsePeakTime=0;
    float m_PulseBrightness=0;
};

using Fx_AllBehaviors = Vector<FxBehavior>;
struct EventSplat
{
    String tex1;
    String tex2;
};
enum GeoEvent_Flags {
    Event_LookAtCamera = 2,
    Event_AdoptParentEntity = 4,
    Event_NoReticle = 8,
};
struct FxSoundData
{
    String m_Name;
    float m_Radius = 100.0f;
    float m_Fade = 20.0f;
    float m_Volume = 1.0f;
};

struct FxGeoEntry_Event
{
    String m_EName;
    String  m_Type;
    String  m_At;
    String  m_Bhvr;
    Vector<Vector<String >> m_Geom;
    int m_AltPiv=0;
    String  m_AnimPiv;
    String  m_Anim;
    String  m_SetState;
    Vector<Vector<String >> m_Part;
    String  m_ChildFx;
    String  m_LookAt;
    String  m_Magnet;
    String  m_POther;
    String  m_PMagnet;
    Vector<EventSplat> m_Splats;
    Vector<FxSoundData> m_Sounds;
    int m_Debris=0;
    String  m_WorldGroup;
    eastl::pair<uint8_t,uint8_t> m_Power;
    float m_LifeSpan=0;
    int m_Flags=0;
    Vector<Vector<String >> m_Until;
    Vector<Vector<String >> m_While;
    // Converted bits.
    SeqBitSet m_UntilBits;
    SeqBitSet m_WhileBits;
};
enum FxCondition_Bits
{
    CondFlag_None           = 0x0,
    CondFlag_Time           = 0x2,
    CondFlag_Primedied      = 0x4,
    CondFlag_Primehit       = 0x8,
    CondFlag_Primebeamhit   = 0x10,
    CondFlag_Cycle          = 0x20,
    CondFlag_Prime1hit      = 0x40,
    CondFlag_Prime2hit      = 0x80,
    CondFlag_Prime3hit      = 0x100,
    CondFlag_Prime4hit      = 0x200,
    CondFlag_Prime5hit      = 0x400,
    CondFlag_Triggerbits    = 0x800,
    CondFlag_Death          = 0x1000,
    CondFlag_DefaultSurface = 0x2000,
    CondFlag_Prime6hit      = 0x4000,
    CondFlag_Prime7hit      = 0x8000,
    CondFlag_Prime8hit      = 0x10000,
    CondFlag_Prime9hit      = 0x20000,
    CondFlag_Prime10hit     = 0x40000,

    CondFlag_AllPrimeHits = CondFlag_Prime1hit | CondFlag_Prime2hit | CondFlag_Prime3hit |
                            CondFlag_Prime4hit | CondFlag_Prime5hit | CondFlag_Primebeamhit | CondFlag_Primehit
};
struct FxInfo_Condition
{
    String  m_On;
    float m_Time;
    int m_Dist;
    float m_Chance;
    int DoMany;
    Vector<FxGeoEntry_Event> m_Event;
    int trigger; // FxCondition_Bits
    Vector<Vector<String >> str_TriggerBits;
    SeqBitSet triggerstates[9];
};
struct FxInfo_Input {
    String  m_InpName;
};
enum FxInfo_Flags {
    FxInfo_SoundOnly=2,
    FxInfo_InheritAlpha=4,
    FxInfo_IsArmor=8,
};
struct FxInfo
{
    Vector<String > inputs;
    Vector<FxInfo_Input> m_Inputs;
    Vector<FxInfo_Condition> m_Conditions;
    String  fxname;
    int m_LifeSpan;
    int m_Lighting;
    FxInfo_Flags m_Flags; //FxInfo_Flags
    float m_PerformanceRadius;
    int hasEventsOnDeath;
    int m_initialized;
    uint8_t initialized() const { return m_initialized &0xFF;}
    int last_change_date;
};
using Fx_AllInfos = Vector<FxInfo>;
