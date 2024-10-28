/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once
#include "Common/Containers/String.h"
#include "Common/Containers/Vector.h"

struct ItemPower_Data
{
    String m_PowerCategory;
    String m_PowerSet;
    String m_Power;
    int m_Level;
    int m_Remove;
};

struct ShopItemInfo_Data
{
    String m_Name;
    ItemPower_Data m_Power;
    int m_Sell;
    int m_Buy;
    int m_CountPerStore;
    Vector<uint32_t> m_Departments;
    struct Power_Data *power_tpl; // looked up by using entries in m_Power
};
using AllShopItems_Data = Vector<ShopItemInfo_Data>;

struct ShopBuySell_Data
{
    int m_Department;
    float m_Markup;
};

struct ShopItem_Data
{
    String m_Name;
};

struct Shop_Data
{
    String m_Name;
    Vector<ShopBuySell_Data> m_Sells;
    Vector<ShopBuySell_Data> m_Buys;
    Vector<ShopItem_Data> m_Items;
};
using AllShops_Data = Vector<Shop_Data>;

struct ShopDeptName_Data
{
    String m_Names;
};
using AllShopDepts_Data = Vector<ShopDeptName_Data>;
