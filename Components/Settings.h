/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2024 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once

#include "Common/Containers/String.h"
#include "Common/Containers/StringView.h"
#include "Common/Containers/Vector.h"

class SettingsImpl;

class Settings
{
public:
    static void        setSettingsPath(const String &path);
    static String      getSettingsPath();
    static void        discoverSEGSDir();
    static String      getSEGSDir(); //!< Returns the absolute path to the SEGS installation directory
    static String      getSettingsTplPath();
    static String      getTemplateDirPath();
    static void        createSettingsFile(const String &new_file_path);

    template<typename T>
    T value(StringView key,const T &default_value, bool *ok=nullptr);

    StringView value(StringView key);

    void                       beginGroup(StringView);
    void                       endGroup();
    [[nodiscard]] Vector<String> childGroups() const;
    [[nodiscard]] Vector<String> allKeys() const;
    bool contains(StringView str);

    explicit Settings(StringView);
    ~Settings();
    Settings(const Settings&) = delete;
    Settings& operator=(const Settings&) = delete;

private:

    SettingsImpl* m_impl;

    static String      s_segs_dir;
    static String      s_settings_path;
    static String      s_default_tpl_dir;
    static String      s_default_settings_path;
};

void settingsDump();
void settingsDump(Settings *s);
bool fileExists(const String &path);
