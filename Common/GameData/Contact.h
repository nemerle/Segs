/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once

#include "Common/Containers/HashMap.h"
#include "Containers/String.h"
#include "Containers/Vector.h"

#include <glm/vec3.hpp>

struct CharacterData;

extern HashMap<String,uint32_t> contactLinkHash;

struct Destination // aka waypoint
{
  public:
    enum : uint32_t {class_version       = 2};

    int point_idx = 0;
    glm::vec3 location;
    String         m_location_name;
    String         m_location_map_name;

    // for scripting language access.
    const String &getLocationName() const { return m_location_name; }
    void setLocationName(const char *n) { m_location_name = n; }

    const String &getLocationMapName() const { return m_location_map_name;}
    void setLocationMapName(const char *n) { m_location_map_name = n; }


    template<class Archive>
    void serialize(Archive &archive, uint32_t const version);

    bool operator==(const Destination &) const = default;
};


class Contact
{
public:
    enum : uint32_t {class_version       = 2};

    // for scripting language access.
    const String &getName() const { return m_name; }
    void setName(const char *n) { m_name = n; }

    const String &getLocationDescription() const { return m_location_description; }
    void setLocationDescription(const char *n) { m_location_description = n; }

    String         m_name;
    String         m_location_description;
    uint32_t        m_npc_id;
    uint32_t        m_contact_idx;
    uint32_t        m_current_standing;
    uint32_t        m_confidant_threshold;
    uint32_t        m_friend_threshold;
    uint32_t        m_complete_threshold;
    uint32_t        m_task_index            = 0;
    bool            m_notify_player         = false;
    bool            m_can_use_cell          = false;
    bool            m_has_location          = false;
    Destination     m_location;

    // Not saved to DB
    uint32_t        m_dlg_screen            = 0;
    bool            m_setting_title         = false;

    bool operator==(const Contact &) const = default;

    template<class Archive>
    void serialize(Archive &archive, uint32_t const version);

};
using vContactList = Vector<Contact>;

struct ContactEntry
{
    String     m_response_text; // char[100]
    uint32_t    m_link = 0;

    template<class Archive>
    void serialize(Archive &ar)
    {
        ar(m_response_text);
        ar(m_link);
    }
};

struct ContactEntryBulk
{
    String     m_msgbody; // char[20000]
    Vector<ContactEntry> m_responses; // must be size 11, or cannot exceed 11?
    // size_t num_active_contacts; // we can use size()
};


