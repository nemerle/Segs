/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once
#include "CommonNetStructures.h"
#include "StateStorage.h"
#include <glm/glm.hpp>

class Entity;

/// @brief Entity collision capsule for accurate physics collision
/// @details Calculated from sequence bounding box. Capsule is oriented along
///          longest axis with radius from other two axes.
/// @performance ~6 bytes per entity, always allocated. Could consider global array with handle stored in entity?
struct EntityCapsule
{
    /// @brief Maximum collision distance from entity origin (game units)
    float getMaxCollisionDistance() const {
        float dist = (m_has_offset ? glm::length(getOffset()) : 0.0f) + getRadius() + getLength();
        if (m_dir != 0)
            dist += getRadius();
        return dist;
    }

           // Helper methods
    float getLength() const { return m_length_mm / 1000.0f; }
    float getRadius() const { return m_radius_mm / 1000.0f; }


    glm::vec3 getOffset() const;
    void setOffset(glm::vec3);

    /// @brief Reset capsule to defaults
    void reset()
    {
        m_length_mm = 0.0f;
        m_radius_mm = 0.0f;
        m_dir = 1;
        m_has_offset = 0;
    }

    void calculateFromBounds(glm::vec3 bounds, glm::vec3 offs);

    /// @brief Length along capsule's main axis (game units)
    /// @lifecycle Calculated from sequence bounds, updated on model change
    uint16_t m_length_mm = 0;

    /// @brief Radius of capsule (game units)
    /// @lifecycle Calculated from sequence bounds
    uint16_t m_radius_mm = 0;

    /// @brief Orientation axis (0=X, 1=Y, 2=Z)
    /// @details Determined by longest bounding box dimension. 1=upright (most common)
    /// @lifecycle Calculated from sequence bounds
    uint8_t m_dir:3       = 1; // Default Y-axis (upright)
    uint8_t m_has_offset:1 = 0;
};

struct PosUpdate // PosUpdatePair
{
    glm::vec3       m_position;
    glm::vec3       m_pyr_angles;
    uint32_t        m_timestamp;
    int             m_debug;
};

struct SurfaceParams
{
    float traction=0;
    float friction=0;
    float bounce=0;
    float gravitational_constant=0;
    float max_speed=0;
};
static_assert(2*sizeof(SurfaceParams)==320/8,"Required since it's sent as an bit array");

/*
 * Global Surface Params
 */
extern SurfaceParams g_world_surf_params[2];

enum CollFlags // TODO: Move to Collision file
{
    COLL_DISTFROMSTART      = 0x1,
    COLL_DISTFROMCENTER     = 0x2,
    COLL_HITANY             = 0x4,
    COLL_NEARESTVERTEX      = 0x8,
    COLL_NODECALLBACK       = 0x10,
    COLL_PORTAL             = 0x20,
    COLL_EDITONLY           = 0x40,
    COLL_BOTHSIDES          = 0x80,
    COLL_CYLINDER           = 0x100,
    COLL_ENTBLOCKER         = 0x200,
    COLL_PLAYERSELECT       = 0x400,
    COLL_NORMALTRI          = 0x800,
    COLL_FINDINSIDE         = 0x1000,
    COLL_FINDINSIDE_ANY     = 0x2000,
    COLL_TRICALLBACK        = 0x4000,
    COLL_GATHERTRIS         = 0x8000,
    COLL_DISTFROMSTARTEXACT = 0x10000,
    COLL_DENSITYREJECT      = 0x20000,
    COLL_IGNOREINVISIBLE    = 0x40000,
    COLL_NOTSELECTABLE      = 0x80000,
    COLL_SURF_SLICK         = 0x100000,
    COLL_SURF_ICY           = 0x200000,
    COLL_SURF_BOUNCY        = 0x400000,
    COLL_SURFACE_FLAGS      = 0x700000,
};

struct TriggeredMove // move to triggeredmove or sequences file later
{
    uint32_t    m_move_idx          = 0;
    uint32_t    m_ticks_to_delay    = 0;
    uint32_t    m_trigger_fx_idx    = 0;
};

enum StuckType
{
    STUCK_NONE          = 0x0,
    STUCK_SLIDE         = 0x1,
    STUCK_COMPLETELY    = 0x2,
};

enum MoveType
{
    MOVETYPE_WALK       = 0x1,
    MOVETYPE_FLY        = 0x2,
    MOVETYPE_NOCOLL     = 0x4,
    MOVETYPE_WIRE       = 0x8,
    MOVETYPE_JETPACK    = 0x10,
};

struct MotionState // current derived state of motion
{
    // ========================================================================
    // HOT DATA: Accessed every physics tick (keep together for cache locality)
    // ========================================================================

    /// @brief Current velocity in world space (game units/tick)
    /// @lifecycle Updated every physics tick by physics system
    /// @thread_safety Physics thread write, rendering thread read
    /// @network SERVER_AUTHORITATIVE - synchronized to clients for correction
    glm::vec3       m_velocity              = {0,0,0};
    /// @brief Input velocity from controls in local space (game units/tick)
    /// @lifecycle Set from control input each tick, consumed by physics, then cleared
    /// @thread_safety Physics thread only
    /// @network CLIENT_PREDICTED - not synchronized (client calculates independently)
    glm::vec3       m_input_velocity        = {0,0,0};
    /// @brief Additional velocity from external forces (game units/tick)
    /// @details Sources: bounce pads, launch pads, entity collisions, knockback
    /// @lifecycle Set by collision/force system, added to velocity in physics, then cleared
    /// @thread_safety Physics thread only
    /// @network CLIENT_PREDICTED - applied deterministically on both sides
    glm::vec3       m_added_vel             = {0,0,0};
    /// @brief Position at start of physics tick (game units)
    /// @lifecycle Captured at start of each physics tick before movement
    /// @thread_safety Physics thread only
    /// @network Not synchronized (used for delta calculation only)
    glm::vec3       m_last_pos              = {0,0,0};

    // Movement state flags - frequently checked
    // TODO: Consider bit packing these in performance optimization pass
    bool            m_is_falling            = false; // freefall
    bool            m_is_stunned            = false; // Stunned/controlled/immobilized
    bool            m_is_flying             = false; // Flying mode active
    bool            m_is_jumping            = false; // Currently in jump arc
    bool            m_is_bouncing           = false; // Bouncing/repulsion active
    bool            m_is_sliding            = false; // Sliding on slippery surface (3-bit in original)
    bool            m_has_jumppack          = false; // Jumppack/jetpack velocity boost active
    bool            m_is_slowed             = false; // Movement slowed by effect
    bool            m_controls_disabled     = false; // Controls disabled (cutscene, etc)
    bool            m_no_collision          = false; //
    /// @brief Physics tick counter (wraps at UINT32_MAX)
    /// @details Increments every motion update call. Used for cooldowns and timing.
    /// @warning Overflows after ~50 days at 1000 ticks/sec. Use wraparound-safe comparison:
    ///          (int32_t)(current - old) to get signed delta that handles wraparound.
    /// @thread_safety Physics thread only
    uint32_t        m_tick_counter          = 0;
    /// @brief Velocity scale multiplier [0.0-∞, typically 0.0-2.0]
    /// @details Applied to input velocity. Affected by powers, debuffs, speed scale option.
    /// @lifecycle Set each tick before physics from control input scale
    float           m_velocity_scale        = 0.0f;

           /// @brief Time spent moving (seconds, for acceleration curves)
    /// @details Accumulates while moving, resets when stopped. Used for nocoll acceleration.
    /// @lifecycle Increments with timestep while input_velocity != 0
    float           m_move_time             = 0.0f;
    /// @brief Speed multipliers per axis [typically 1.0]
    /// @details Modified by powers, effects. Applied to input velocity.
    /// @lifecycle Set by power system, defaults to (1,1,1)
    glm::vec3       m_speed                 = {1,1,1};
    /// @brief Backward movement speed multiplier [0.0-1.0, typically 0.5-1.0]
    /// @details Applied when moving backward. Most entities slower when backpedaling.
    /// @lifecycle Set from character attributes
    float           m_backup_spd            = 1.0f;
    // ========================================================================
    // JUMP DATA: Accessed during jump processing
    // Grouped together for spatial locality when processing jump logic
    // ========================================================================
    /// @brief Whether jump button is currently held down
    /// @details Used for variable jump height: hold=full height, tap=short hop
    /// @lifecycle Set from control input each frame
    /// @thread_safety Physics thread only
    bool            m_jump_still_held       = false;
    /// @brief Base jump height multiplier [typically 0.0-10.0, default 2.0]
    /// @details Modified by powers (Jump Pack, etc). Final height = this * 4.0 game units.
    /// @lifecycle Set from character attributes, modified by powers
    float           m_jump_height           = 2.0f;
    /// @brief Maximum height for current jump (game units)
    /// @details Calculated as jump_start + (jump_height * 4.0) when jump begins.
    /// @lifecycle Set when jump starts, checked each tick to end jump
    float           m_max_jump_height       = 0.0f;
    /// @brief Highest Y position of last completed jump (game units)
    /// @details Used for analytics/achievements, not for gameplay logic.
    /// @lifecycle Set on landing
    float           m_jump_apex             = 0.0f;
    /// @brief Y position where current jump started (game units)
    /// @details Compared with current Y to determine if max jump height reached.
    /// @lifecycle Set when jump starts (when leaving ground)
    float           m_jump_start            = 0.0f;
    /// @brief Ticks remaining with jump boost [0-15]
    /// @details Counts down from 15. While >0, upward velocity is maintained.
    /// @lifecycle Set to 15 when jump starts, decrements each tick
    int             m_jump_time             = 0;
    // ========================================================================
    // FALL TRACKING: For fall damage calculation and animation triggers
    // ========================================================================
    /// @brief Highest Y position reached since last ground touch (game units)
    /// @details Tracks apex while airborne. Used to calculate fall damage on landing.
    /// @lifecycle Reset on landing, updated while m_is_falling=true
    /// @thread_safety Physics thread only
    float           m_highest_height        = 0.0f;

    /// @brief Y position when last touching ground (game units)
    /// @details Compared with current Y during fall to trigger BIGFALL animation (>3.5 units).
    /// @lifecycle Set every time entity lands
    /// @thread_safety Physics thread only
    float           m_height_last_ground    = 0.0f;

    /// @brief Legacy: Currently boolean, will become fall recovery timer (int)
    /// @details Will track landing recovery stun duration (ticks).
    bool            m_has_headpain          = false;

    // SURFACE INTERACTION: Surface physics and environmental effects
    // Accessed during surface parameter calculation and physics
    // ========================================================================

    /// @brief Primary surface normal vector (usually points up from ground)
    /// @lifecycle Set during collision detection
    glm::vec3       m_surf_normal           = {0,1,0}; // Default: flat ground

    glm::vec3       m_surf_normal2          = {0,0,0}; ///< Secondary surface (multi-surface contact)
    glm::vec3       m_surf_normal3          = {0,0,0}; ///< Tertiary surface (rare, complex geometry)

    /// @brief Surface repulsion force vector (game units/tick)
    /// @details Calculated during collision, added to velocity to push away from surfaces.
    /// @lifecycle Set during world collision, consumed in physics
    glm::vec3       m_surf_repulsion        = {0,0,0};

    /// @brief Surface parameter modifiers [0]=ground physics, [1]=air physics
    /// @details Multiplied with base surface params. Modified by powers (Ice Slick, etc).
    /// @lifecycle Set by power/effect system, applied during physics
    SurfaceParams   m_surf_mods[2]          = {{0,0,0,0,0}, {0,0,0,0,0}};
    /// @brief Surface flags from last collision (COLL_SURF_SLICK | COLL_SURF_ICY | COLL_SURF_BOUNCY)
    /// @details Applied as texture overrides to base surface parameters.
    /// @lifecycle Set during world collision, used in entWorldGetSurface()
    int             m_last_surf_flags       = 0;
    // Traction effects (from surfaces and combat)

    /// @brief Remaining ticks of low traction effect [0-N]
    /// @details Counts down each tick. While >0, reduced traction applied.
    /// @lifecycle Set by effect system, decrements in motion processing
    int             m_low_traction_steps    = 0;
    /// @brief Traction loss percentage from combat hits [0.0-1.0]
    /// @details 0.0=no loss, 1.0=complete loss. Multiplied with surface traction.
    ///          Recovers at 0.04/tick if <0.9, or 0.001/tick if ≥0.9 (heavy hits recover slower).
    /// @lifecycle Set on hit, auto-recovers each tick, clamped to [0.0, 1.0]
    /// @thread_safety Physics thread only
    float           m_hit_stumble_loss      = 0.0f;
    // ========================================================================
    // COLLISION DATA: Used during collision detection (colder path)
    // Accessed less frequently than velocity/position
    // ========================================================================

    /// @brief Entity collision capsule dimensions
    /// @details Calculated from sequence bounds. Updated on model/costume change.
    /// @performance 6 bytes, always allocated. Could be pointer for non-colliding entities.
    /// @lifecycle Calculated when sequence loads, updated on model/seq change
    EntityCapsule   m_capsule;
    /// @brief Entity ID of last entity collision [0 = no recent collision]
    /// @details Used for collision cooldown to prevent repeated bounce/launch.
    /// @lifecycle Set during entity collision check, compared for cooldown
    int             m_last_ent_collided     = 0;
    /// @brief Tick of last entity collision (for cooldown calculation)
    /// @details Use wraparound-safe comparison: (int32_t)(m_tick_counter - m_tick_last_ent_coll)
    /// @lifecycle Set during entity collision check, compared with current tick
    /// @warning Tick counter wraps at UINT32_MAX - use signed delta for comparison
    uint32_t        m_tick_last_ent_coll    = 0;

    int             m_walk_flags            = 0;        ///< Walk mode flags (TODO: document flags)
    int             m_coll_surf_flags       = 0;        ///< Collision surface flags (TODO: document flags)

    StuckType       m_stuck                 = StuckType::STUCK_NONE;      ///< Body stuck state
    StuckType       m_stuck_head            = StuckType::STUCK_NONE;      ///< Head stuck (ceiling collision)

    // ========================================================================
    // STATE MANAGEMENT: Network sync and versioning
    // ========================================================================

    uint8_t         m_motion_state_id       = 1;        ///< Motion state version ID (for network sync)
    bool            m_update_motion_state   = true;     ///< Needs network update flag

    /// @brief Enable detailed movement logging for this entity
    /// @performance Significant log spam if enabled - use sparingly
    bool            m_debug                 = false;

    // ========================================================================
    // HELPER METHODS: Convenience functions for common operations
    // These improve maintainability and reduce errors from manual field manipulation
    // ========================================================================

    /// @brief Reset all fields to default values
    /// @details Use when recycling entity or resetting state
    void reset();
    /// @brief Check if entity collision cooldown has expired
    /// @param entity_id Entity to check cooldown for
    /// @param cooldown_ticks Cooldown duration in ticks (default: 30 for most collisions)
    /// @return true if can collide with this entity again
    bool canCollideWithEntity(int entity_id, int32_t cooldown_ticks = 30) const;

    /// @brief Start collision cooldown for an entity
    /// @param entity_id Entity that was collided with
    void startCollisionCooldown(int entity_id);

    /// @brief Apply hit stumble effect from combat
    /// @param stumble_amount Traction loss [0.0-1.0], where 1.0 = complete loss
    /// @details Does NOT stack - takes maximum of current and new value
    void applyHitStumble(float stumble_amount);
    /// @brief Update fall tracking while airborne
    /// @param current_y Current Y position (game units)
    /// @details Call every tick while falling to track highest point
    void updateFallTracking(float current_y);
    /// @brief Get distance fallen from highest point
    /// @param current_y Current Y position (game units)
    /// @return Fall distance in game units (0 if not falling)
    float getFallDistance(float current_y) const;

    /// @brief Check if this is a "big fall" that triggers BIGFALL animation
    /// @param current_y Current Y position (game units)
    /// @return true if fallen >3.5 units from last ground touch
    bool isBigFall(float current_y) const;

    /// @brief Validate motion state invariants (for debugging)
    /// @return true if all fields are in valid ranges
    /// @details Call in debug builds to catch corruption early
    bool isValid() const;
#ifdef _DEBUG
    /// @brief Get debug string representation (debug builds only)
    /// @return Human-readable state summary
    eastl::string toDebugString() const
    {
        char buf[512];
        std::snprintf(buf, sizeof(buf),
                      "Motion[tick=%u, vel=(%.2f,%.2f,%.2f), fall=%d, jump=%d, stumble=%.2f]",
                      m_tick_counter,
                      m_velocity.x, m_velocity.y, m_velocity.z,
                      m_is_falling ? 1 : 0,
                      m_is_jumping ? 1 : 0,
                      m_hit_stumble_loss
                      );
        return std::string(buf);
    }
#endif

};

/// Process queued input changes and run physics if enough time has accumulated
/// @param e The entity to process
/// @param delta_time_sec Time elapsed since last call (seconds)
///                       Will be clamped to [0, MAX_DELTA_TIME_SEC]
void processNewInputs(Entity &e, float delta_time_sec);

void addPosUpdate(Entity &e, const PosUpdate &p);
void forcePosition(Entity &e, glm::vec3 pos);
void forceOrientation(Entity &e, glm::vec3 pyr);

// Move to Sequences or Triggers files later
void addTriggeredMove(Entity &e, uint32_t move_idx, uint32_t delay, uint32_t fx_idx);
