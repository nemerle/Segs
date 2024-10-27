/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

/*!
 * @addtogroup NetStructures Projects/CoX/Common/NetStructures
 * @{
 */

#include "CharacterHelpers.h"
#include "Character.h"
#include "Costume.h"
#include <chrono>
#include <format>

/*
 * Character Methods
 */
// Getter
uint32_t            getLevel(const Character &c) { return c.m_char_data.m_level; }
uint32_t            getCombatLevel(const Character &c) { return c.m_char_data.m_combat_level; }
uint32_t            getSecurityThreat(const Character &c) { return c.m_char_data.m_security_threat; }
float               getHP(const Character &c) { return c.m_char_data.m_current_attribs.m_HitPoints; }
float               getEnd(const Character &c) { return c.m_char_data.m_current_attribs.m_Endurance; }
float               getMaxHP(const Character &c) { return c.m_max_attribs.m_HitPoints; }
float               getMaxEnd(const Character &c) { return c.m_max_attribs.m_Endurance; }
uint32_t            getCurrentCostumeIdx(const Character &c) { return c.m_char_data.m_current_costume_idx; }
const String &     getOrigin(const Character &c) { return c.m_char_data.m_origin_name; }
const String &     getClass(const Character &c) { return c.m_char_data.m_class_name; }
uint32_t            getXP(const Character &c) { return c.m_char_data.m_experience_points; }
uint32_t            getDebt(const Character &c) { return c.m_char_data.m_experience_debt; }
uint32_t            getPatrolXP(const Character &c) { return c.m_char_data.m_experience_patrol; }
const String &     getGenericTitle(const Character &c) { return c.m_char_data.m_titles[0]; }
const String &     getOriginTitle(const Character &c) { return c.m_char_data.m_titles[1]; }
const String &     getSpecialTitle(const Character &c) { return c.m_char_data.m_titles[2]; }
uint32_t            getInf(const Character &c) { return c.m_char_data.m_influence; }
const String &     getDescription(const Character &c) { return c.m_char_data.m_character_description ; }
const String &     getBattleCry(const Character &c) { return c.m_char_data.m_battle_cry; }
const String &     getAlignment(const Character &c) { return c.m_char_data.m_alignment; }
const String &     getLastOnline(const Character &c) { return c.m_char_data.m_last_online; }

// Setters
void setLevel(Character &c, uint32_t val)
{
    GameDataStore &data(getGameData());
    if(val > data.expMaxLevel())
        val = data.expMaxLevel();
    c.m_char_data.m_level = val; // client stores lvl arrays starting at 0
    c.finalizeLevel();
    setHPToMax(c);
    setEndToMax(c);
}

void setCombatLevel(Character &c, uint32_t val)
{
    GameDataStore &data(getGameData());
    if(val > data.expMaxLevel())
        val = data.expMaxLevel();
    c.m_char_data.m_combat_level = val;
    c.finalizeCombatLevel();
}

void setSecurityThreat(Character &c, uint32_t val)
{
    GameDataStore &data(getGameData());
    if(val > data.expMaxLevel())
        val = data.expMaxLevel();
    c.m_char_data.m_security_threat = val;
}

void setHP(Character &c, float val)
{
    c.m_char_data.m_current_attribs.m_HitPoints = std::max(0.0f, std::min(val,c.m_max_attribs.m_HitPoints));
}

void setHPToMax(Character &c)
{
    setHP(c, getMaxHP(c));
}

void setEnd(Character &c, float val)
{
    c.m_char_data.m_current_attribs.m_Endurance = std::max(0.0f, std::min(val,c.m_max_attribs.m_Endurance));
}

void setEndToMax(Character &c)
{
    setEnd(c, getMaxEnd(c));
}

void setCurrentCostumeIdx(Character &c, uint32_t idx)
{
    c.m_add_new_costume = true;
    c.m_char_data.m_current_costume_idx = idx;
}

void setXP(Character &c, uint32_t val)
{
    c.m_char_data.m_experience_points = val;
}

void setDebt(Character &c, uint32_t val)
{
    c.m_char_data.m_experience_debt = val;
}

void setTitles(Character &c, bool prefix, StringView generic, StringView origin, StringView special)
{
    // if "NULL", clear string
    if(generic=="NULL")
        generic=StringView();
    if(origin=="NULL")
        origin=StringView();
    if(special=="NULL")
        special=StringView();

    c.m_char_data.m_has_titles = prefix || !generic.empty() || !origin.empty() || !special.empty();
    if(!c.m_char_data.m_has_titles)
      return;

    c.m_char_data.m_has_the_prefix = prefix;
    c.m_char_data.m_titles[0] = generic;
    c.m_char_data.m_titles[1] = origin;
    c.m_char_data.m_titles[2] = special;
}

void setInf(Character &c, uint32_t val)
{
    c.m_char_data.m_influence = val;
}

void setDescription(Character &c, StringView val)
{
    c.m_char_data.m_character_description = val;
}

void setBattleCry(Character &c, StringView val)
{
    c.m_char_data.m_battle_cry = val;
}

void setAFK(Character &c, const bool is_afk, StringView msg)
{
    c.m_char_data.m_afk = is_afk;
    if(is_afk)
        c.m_char_data.m_afk_msg = msg;
}
bool isAFK(Character &c)
{
    return c.m_char_data.m_afk;
}

void initializeCharacter(Character &c)
{
    GameDataStore &data(getGameData());
    int entclass = getEntityClassIndex(data, true, c.m_char_data.m_class_name);
    c.m_char_data.m_current_attribs = data.m_player_classes[entclass].m_AttribBase[0];
    c.m_char_data.m_current_attribs.m_HitPoints = c.m_max_attribs.m_HitPoints;
    c.m_char_data.m_current_attribs.m_Endurance = c.m_max_attribs.m_Endurance;
    c.m_char_data.m_current_attribs.m_Regeneration *=4;     //for some reason the base regen rate is .25
}

void updateLastOnline(Character &c)
{
    //"Wed May 20 03:40:13 1998"
    auto now = std::chrono::system_clock::now();
    auto formatted= std::format("{:%Y-%m-%d %X}", now);
    c.m_char_data.m_last_online = String(formatted.c_str());
}

// Toggles
void toggleAFK(Character &c, StringView msg)
{
    if(msg.empty())
        msg = "AFK";

    setAFK(c, !c.m_char_data.m_afk, msg);
}


/*
 * Titles -- TODO: get titles from texts/English/titles_def
 */
static const eastl::fixed_vector<StringView,20> g_generic_titles =
{
    "NULL",
    "Awesome",
    "Bold",
    "Courageous",
    "Daring",
    "Extraordinary",
    "Famous",
    "Gallant",
    "Heroic",
    "Incomparable",
    "Legendary",
    "Magnificent",
    "Outstanding",
    "Powerful",
    "Remarkable",
    "Startling",
    "Terrific",
    "Ultimate",
    "Valiant",
    "Wonderful",
};

// TODO: get titles from texts/English/titles_def
static const eastl::fixed_vector<StringView,20> g_origin_titles =
{
    "NULL",
    "Adept",
    "Bright",
    "Curious",
    "Deductiv",
    "Exceptional",
    "Far Seeing",
    "Glorious",
    "Honorable",
    "Indescribable",
    "Lucky",
    "Majestic",
    "Otherworldly",
    "Phenomenal",
    "Redoubtable",
    "Stupendous",
    "Thoughtful",
    "Unearthly",
    "Venturous",
    "Watchful",
};

StringView getGenericTitle(uint32_t val)
{
    return g_generic_titles.at(val);
}

StringView getOriginTitle(uint32_t val)
{
    return g_origin_titles.at(val);
}

//! @}
