/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once
#include <stdint.h>
#include <Common/Containers/String.h>

namespace SEGS
{
class IFilesystem;
}

class BinStore;
struct SceneGraph_Data;
struct FSWrapper;

static constexpr uint32_t scenegraph_i0_2_requiredCrc=0xD3432007;
bool loadFrom(BinStore *s,SceneGraph_Data &target);
bool loadFrom(const String &filepath, SceneGraph_Data &target);
void saveTo(const SceneGraph_Data &target,const String &baseName,bool text_format=false);
//TODO: move getFilepathCaseInsensitive to a saner place
String getFilepathCaseInsensitive(SEGS::IFilesystem *fs, const String &fpath);
//! Generic loader function will load cereal version, or if that does not exists a bin version
bool LoadSceneData(const String &fname, SceneGraph_Data &scenegraph);
