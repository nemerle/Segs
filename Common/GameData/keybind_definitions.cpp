/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

/*!
 * @addtogroup GameData Projects/CoX/Common/GameData
 * @{
 */

#include "keybind_definitions.h"

#include "GameDataStore.h"
#include "Components/Logging.h"

String makeKeyString(const KeyName &key, const ModKeys &mods)
{
    String nullstr;
    String k = keyNameToEnum.key(key,nullstr);
    String m = modNameToEnum.key(mods,nullstr);
    String keystring;

    if(m != nullptr)
        keystring = m+"+"+k;
    else
        keystring = k;

    //qCDebug(logKeybinds) << keystring << k << m << key << mods;

    return keystring.to_lower();
}

KeybindSettings::KeybindSettings()
{
//    resetKeybinds();
}

void KeybindSettings::setKeybindProfile(String &profile)
{
    m_cur_keybind_profile = profile;
}

const CurrentKeybinds &KeybindSettings::getCurrentKeybinds() const
{
    GameDataStore &data(getGameData());
    assert(!data.m_keybind_profiles.empty()); // just incase GameData fails us
    if(m_keybind_profiles.empty())
        return data.m_keybind_profiles.front().KeybindArr;

    for(const auto &p : m_keybind_profiles)
    {
        if(p.Name == m_cur_keybind_profile)
            return p.KeybindArr;
    }

    sCDebug(logKeybinds) << "Could not get Current Keybinds. Returning first keybind profile.";
    return m_keybind_profiles.front().KeybindArr;
}

void KeybindSettings::resetKeybinds(const Parse_AllKeyProfiles &default_profiles)
{
    m_keybind_profiles = default_profiles;
}

void KeybindSettings::setKeybind(String &profile, KeyName &key, ModKeys &mods, String &command, bool &is_secondary)
{
    removeKeybind(profile,key,mods); // remove previous keybinds

    String keystring = makeKeyString(key,mods); // Construct keystring from mods+key

    for(auto &p : m_keybind_profiles)
    {
        if(p.Name == profile)
            p.KeybindArr.push_back({key,mods,keystring,command,is_secondary});
    }

    sCDebug(logKeybinds) << "Setting keybind: " << profile << key << mods << keystring << command << is_secondary;

}

void KeybindSettings::removeKeybind(String &profile, KeyName &key, ModKeys &mods)
{
    String keystring = makeKeyString(key,mods); // Construct keystring from mods+key

    for(auto &p : m_keybind_profiles)
    {
        if(p.Name != profile)
            continue;

        for(auto iter=p.KeybindArr.begin(); iter!=p.KeybindArr.end(); /*incremented inside loop*/)
        {
          if(iter->Key==key && iter->Mods==mods)
              iter=p.KeybindArr.erase(iter);
          else
              ++iter;
        }
    }
    sCDebug(logKeybinds) << "Clearing keybind: " << profile << key << mods << keystring;
}

void KeybindSettings::keybindsDump()
{
    sDebug() << "Debugging Keybinds:"
                       << "\n\t" << "Current Profile Name:" << m_cur_keybind_profile;

    for(const auto &profile : m_keybind_profiles)
    {
        sDebug() << profile.DisplayName << profile.Name;

        for(const auto &k : profile.KeybindArr)
            sDebug() << k.KeyString << k.Key << k.Mods << k.Command << k.IsSecondary;
    }
}

//! @}
