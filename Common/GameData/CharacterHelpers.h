/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once

#include "Common/Containers/String.h"
#include "Common/Containers/StringView.h"
#include <stdint.h>

class Character;
struct EntityData;
struct Friend;

/*
 * Character Methods
 */
// Getters
uint32_t            getLevel(const Character &c);
uint32_t            getCombatLevel(const Character &c);
uint32_t            getSecurityThreat(const Character &c);
float               getHP(const Character &c);
float               getEnd(const Character &c);
float               getMaxHP(const Character &c);
float               getMaxEnd(const Character &c);
uint32_t            getCurrentCostumeIdx(const Character &c);
const String &     getOrigin(const Character &c);
const String &     getClass(const Character &c);
uint32_t            getXP(const Character &c);
uint32_t            getDebt(const Character &c);
uint32_t            getPatrolXP(const Character &c);
const String &     getGenericTitle(const Character &c);
const String &     getOriginTitle(const Character &c);
const String &     getSpecialTitle(const Character &c);

uint32_t            getInf(const Character &c);
const String &     getDescription(const Character &c);
const String &     getBattleCry(const Character &c);
const String &     getAlignment(const Character &c);
const String &     getLastOnline(const Character &c);
//======================================================
// Accessor functions
//======================================================
void    setLevel(Character &c, uint32_t val);
void    setCombatLevel(Character &c, uint32_t val);
void    setSecurityThreat(Character &c, uint32_t val);
void    setHP(Character &c, float val);
void    setEnd(Character &c, float val);
void    setHPToMax(Character &c);
void    setEndToMax(Character &c);
void    setCurrentCostumeIdx(Character &c, uint32_t idx);
void    setXP(Character &c, uint32_t val);
void    setDebt(Character &c, uint32_t val);
void    setTitles(Character &c, bool prefix = false, StringView generic = "", StringView origin = "", StringView special = "");
void    setInf(Character &c, uint32_t val);
void    setDescription(Character &c, StringView val);
void    setBattleCry(Character &c, StringView val);
void    setAFK(Character &c, const bool is_afk, StringView msg = "");
bool    isAFK(Character &c);
void    updateLastOnline(Character &c);
void    initializeCharacter(Character &c);

// Toggles
void    toggleAFK(Character &c, StringView msg = "");

/*
 * Titles -- TODO: get titles from texts/English/titles_def
 */
StringView getGenericTitle(uint32_t val);
StringView getOriginTitle(uint32_t val);
