/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2026 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once

namespace SEGS {

class RootFilesystem;

// Initialize game data mounts - call after settings are available
// Reads coh_install_dir from settings, mounts PIGGs and local overrides
bool initializeGameDataMounts(RootFilesystem* fs);

}
