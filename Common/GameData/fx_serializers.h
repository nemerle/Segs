/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once

#include <stdint.h>
#include <Common/Containers/Vector.h>
#include <Common/Containers/String.h>
class BinStore;
struct ColorFx;
using Fx_AllBehaviors = Vector<struct FxBehavior>;
using Fx_AllInfos = Vector<struct FxInfo>;

constexpr const static uint32_t fxbehaviors_i0_requiredCrc = 0x0DD5777C;
bool loadFrom(BinStore *s,Fx_AllBehaviors &target) ;
bool LoadFxBehaviorData(const String &fname, Fx_AllBehaviors &behaviors);
void saveTo(const Fx_AllBehaviors &target,const String &baseName,bool text_format=false);

constexpr const static uint32_t fxinfos_i0_requiredCrc = 0xB178A55D;
bool loadFrom(BinStore *s,Fx_AllInfos &target);
bool LoadFxInfoData(const String &fname, Fx_AllInfos &infos);
void saveTo(const Fx_AllInfos &target,const String &baseName,bool text_format=false);

template<class Archive>
void serialize(Archive & archive, ColorFx & m);
