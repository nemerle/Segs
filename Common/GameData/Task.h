/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once

#include "CommonNetStructures.h"
#include "Contact.h"
#include "glm/vec3.hpp"
#include "cereal/cereal.hpp"
#include "Components/Logging.h"

class Task
{
public:
    //static const constexpr uint32_t class_version = 1;
    enum : uint32_t {class_version = 2};

    int             m_db_id;
    String          m_description;
    String          m_owner;
    String          m_state;
    String          m_detail;
    bool            m_is_complete = false;
    bool            m_in_progress_maybe = false;
    bool            m_is_abandoned = false;
    bool            m_has_location = false;
    bool            m_detail_invalid = true;
    bool            m_board_train = false;

    Destination     m_location;
    int             m_finish_time;

    //unknown
    int             m_unknown_1;
    int             m_unknown_2;


    //Not listed in Class but expected
    int             m_task_idx;

    // for scripting language access
    const String &getDescription() const { return m_description; }
    void setDescription(const char *n) { m_description = n; }
    const String &getOwner() const { return m_owner;}
    void setOwner(const char *n) { m_owner = n; }
    const String &getState() const { return m_state;}
    void setState(const char *n) { m_state = n; }
    const String &getDetail() const { return m_detail;}
    void setDetail(const char *n) { m_detail = n; }

    template<class Archive>
    void serialize(Archive &archive, uint32_t const version);

    bool operator==(const Task &) const = default;

};

using vTaskList = Vector<Task>;

class TaskEntry
{
public:
    //static const constexpr uint32_t class_version = 1;
    enum : uint32_t {class_version = 2};

    uint32_t m_db_id;
    vTaskList m_task_list;
    bool m_reset_selected_task = true;

    template<class Archive>
    void serialize(Archive &archive, uint32_t const version);
};

using vTaskEntryList = Vector<TaskEntry>;

class TaskObjectiveTimer // Shouldn't serialze to DB?
{
public:
    String m_message;
    float m_mission_time;
};
