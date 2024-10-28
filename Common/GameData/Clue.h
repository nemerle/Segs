/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once
#include "Common/Containers/StringView.h"
#include "Common/Containers/String.h"
#include "Components/Logging.h"
#include "cereal/cereal.hpp"



class Clue
{
public:
     enum : uint32_t {class_version       = 1};

    String m_name;
    String m_display_name;
    String m_detail_text;
    String m_icon_file;

    // for scripting language access.
    const String & getName() const { return m_name;}
    void setName(const String & n) { m_name = n; }

    const String & getDisplayName() const { return m_display_name;}
    void setDisplayName(const String & n) { m_display_name = n; }

    const String & getDetailText() const { return m_detail_text;}
    void setDetailText(const String & n) { m_detail_text = n; }

    const String & getIconFile() const { return m_icon_file;}
    void setIconFile(const String & n) { m_icon_file = n; }


    template<class Archive>
    void serialize(Archive &archive, uint32_t const version);
};
using vClueList = Vector<Clue>;


class Souvenir
{
public:
     enum : uint32_t {class_version       = 1};

    int32_t m_idx;
    String m_name;
    String m_icon;
    String m_description;

    // for scripting language access.
    const String &getName() const { return m_name;}
    void        setName(const char *n) { m_name = n; }

    const String &getIcon() const { return m_icon;}
    void        setIcon(const char *n) { m_icon = n; }

    const String &getDescription() const { return m_description;}
    void        setDescription(const char *n) { m_description = n; }

   template<class Archive>
   void serialize(Archive &archive, uint32_t const version);

};

using vSouvenirList = Vector<Souvenir>;
