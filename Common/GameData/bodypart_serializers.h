/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once


#include <cstdint>
#include "Common/Containers/String.h"

struct BodyPartsStorage;
class BinStore;
struct BodyPart_Data;

constexpr const static uint32_t bodyparts_i0_requiredCrc = 0x541B59EB;
bool loadFrom(BinStore *s,BodyPartsStorage &target) ;
void saveTo(const BodyPartsStorage &target,const String &baseName,bool text_format=false);

