/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once
#include "shop_definitions.h"
#include <stdint.h>

class BinStore;

typedef Vector<struct ShopItemInfo_Data> AllShopItems_Data;
typedef Vector<struct Shop_Data>         AllShops_Data;
typedef Vector<struct ShopDeptName_Data> AllShopDepts_Data;

static constexpr uint32_t shoplist_i0_requiredCrc=0x5418912E;
static constexpr uint32_t shoplist_i24_requiredCrc=0xF249DD6C;
bool loadFrom(BinStore *s, AllShops_Data &target);
void saveTo(const AllShops_Data &target,const String &baseName, bool text_format=false);

static constexpr uint32_t shopitems_i0_requiredCrc=0xAE135EB6;
static constexpr uint32_t shopitems_i24_requiredCrc=0x8753FF10;
bool loadFrom(BinStore *s, AllShopItems_Data &target);
void saveTo(const AllShopItems_Data &target,const String &baseName, bool text_format=false);

static constexpr uint32_t shopdepts_i0_requiredCrc=0xCB0532EC;
static constexpr uint32_t shopdepts_i24_requiredCrc=0x5A060975;
bool loadFrom(BinStore *s, AllShopDepts_Data &target);
void saveTo(const AllShopDepts_Data &target,const String &baseName, bool text_format=false);
