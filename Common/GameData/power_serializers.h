/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once
#include "Containers/String.h"
#include <stdint.h>

class BinStore;
struct AllPowerCategories;

static constexpr uint32_t powers_i0_requiredCrc=0xA53BBC94;
static constexpr uint32_t powers_i24_requiredCrc=0xB553B8FF;
static constexpr uint32_t powersets_i24_requiredCrc=0x2FE0D52D;
static constexpr uint32_t powercats_i24_requiredCrc=0x59248ECB;
bool loadFrom(BinStore *s, AllPowerCategories &target);
bool loadFrom(const String &filepath, AllPowerCategories &target);
void saveTo(const AllPowerCategories &target,const String &baseName,bool text_format=false);
