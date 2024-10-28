/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once
#include <stdint.h>
#include "Common/Containers/String.h"
struct EntityData;

template<class Archive>
void serialize(Archive & archive, EntityData & m, uint32_t const version);

void saveTo(const EntityData &target, const String &baseName, bool text_format=false);

void serializeToDb(const EntityData &data, String &tgt);
void serializeFromDb(EntityData &data, const String &src);
