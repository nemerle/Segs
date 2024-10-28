/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once

#include "Containers/HashMap.h"
#include "Containers/String.h"
#include "Containers/Vector.h"
#include "glm/mat4x4.hpp"

enum class EntType : uint8_t;
struct NpcTemplate
{
    String m_costume_name;
    EntType m_type;
    Vector<glm::mat4> m_initial_positions;
    // attributes/powers ?
};

struct NpcGenerator
{
    String              m_generator_name;
    EntType             m_type;
    Vector<glm::mat4>   m_initial_positions;
    Vector<NpcTemplate> m_possible_npcs;
    void                generate(class MapInstance *);
};

struct NpcGeneratorStore
{
    HashMap<String, NpcGenerator> m_generators;
    void generate(class MapInstance *instance);
};

String makeReadableName(const String &name);
