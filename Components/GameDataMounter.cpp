/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2026 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#include "GameDataMounter.h"
#include "Settings.h"
#include "Logging.h"
#include "Common/Utils/FilesystemHandler.h"

namespace SEGS {

bool initializeGameDataMounts(RootFilesystem* fs)
{
    // Read coh_install_dir from settings
    Settings config(Settings::getSettingsPath());
    config.beginGroup("GameData");
    String coh_install_dir = config.value<String>("coh_install_dir", "");
    config.endGroup();

    if (coh_install_dir.empty()) {
        sWarning() << "coh_install_dir not configured in [GameData] section";
        return false;
    }

    // Mount CoH installation directory
    if (!fs->mount(coh_install_dir, "coh_install:", -99)) {
        sCritical() << "Failed to mount CoH installation:" << coh_install_dir;
        return false;
    }
    sInfo() << "Mounted CoH installation at coh_install:";

    // Mount all PIGG files from coh_install:piggs/
    String piggs_path = "coh_install:piggs";
    int pigg_count = 0;

    fs->visitEntries(piggs_path, [&](StringView entry, bool is_dir) -> VisitResult {
        if (is_dir) {
            return VisitResult::VisitNext;
        }

        String entry_str(entry.data(), entry.size());
        if (!entry_str.ends_with(".pigg")) {
            return VisitResult::VisitNext;
        }

        String pigg_full_path = piggs_path + "/" + entry_str;

        if (fs->mount(pigg_full_path, "coh_data:", -99)) {
            ++pigg_count;
            sCDebug(logSettings) << "Mounted PIGG:" << entry_str;
        } else {
            sWarning() << "Failed to mount PIGG:" << pigg_full_path;
        }

        return VisitResult::VisitNext;
    });

    if (pigg_count == 0) {
        sWarning() << "No PIGG files found in" << piggs_path;
    } else {
        sInfo() << "Mounted" << pigg_count << "PIGG archives to coh_data:";
    }

    // Mount local coh_data/ override directory with higher priority
    String local_coh_data = "app:coh_data";
    String local_path = fs->resolveToNativePath(local_coh_data);
    if (!local_path.empty()) {
        if (fs->mount(local_path, "coh_data:", 0)) {
            sInfo() << "Mounted local coh_data/ override directory";
        }
    }

    return true;
}

}
