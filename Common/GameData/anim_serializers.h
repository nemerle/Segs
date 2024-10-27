/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once
#include "Containers/String.h"

class BinStore;
struct TextureAnim_Data;

template<class Archive>
void serialize(Archive & archive, TextureAnim_Data & m);

bool loadFrom(BinStore *s,TextureAnim_Data &target);
void saveTo(const TextureAnim_Data &target,const String &baseName,bool text_format=false);
