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

#include "keybind_serializers.h"
#include "Components/serialization_common.h"
#include "Components/serialization_types.h"
#include "Containers/Set.h"
#include "keybind_definitions.h"

#include "Components/Logging.h"
#include "DataStorage.h"

namespace
{
    KeyName resolveKey(const String &name)
    {
        auto iter = keyNameToEnum.find(name.to_upper());
        if(iter!=keyNameToEnum.end())
            return iter->second;
        return COH_INPUT_INVALID;
    }

    ModKeys resolveMod(const String &name)
    {
        auto iter = modNameToEnum.find(name.to_upper());
        if(iter!=modNameToEnum.end())
            return iter->second;
        return NO_MOD;
    }

    Vector<Keybind> correctSecondaryBinds(Vector<Keybind> &keybinds)
    {
        Set<String> known_commands;
        for(Keybind & bind : keybinds)  {
            if(known_commands.contains(bind.Command)) {
                sCDebug(logKeybinds) << "Found duplicate keybind " + bind.Command + " Setting to alternate.";
                bind.IsSecondary = true;
            }
            else {
                known_commands.insert(bind.Command);
            }
        }
        return keybinds;
    }

    bool loadFrom(BinStore * s, Keybind & target)
    {
        s->prepare();
        bool ok = true;
        ok &= s->read(target.KeyString);
        ok &= s->read(target.Command);
        ok &= s->prepare_nested(); // will update the file size left

        Vector<String> combo = target.KeyString.split('+');

        if(combo.size() > 1)
        {
            target.Mods = resolveMod(combo.at(0));
            target.Key  = resolveKey(combo.at(1));
        }
        else
            target.Key = resolveKey(target.KeyString);

        sCDebug(logKeybinds) << "\tbind:" << target.KeyString << target.Key << target.Mods << target.Command;

        assert(ok && s->end_encountered());
        return ok;
    }

    bool loadFrom(BinStore * s, Keybind_Profiles & target)
    {
        s->prepare();
        bool ok = true;
        ok &= s->read(target.DisplayName);
        ok &= s->read(target.Name);
        ok &= s->prepare_nested(); // will update the file size left
        if(s->end_encountered())
            return ok;

        sCDebug(logKeybinds) << "Loading Profile:" << target.DisplayName << target.Name;

        String _name;
        while(s->nesting_name(_name))
        {
            s->nest_in();
            if("KeyBind"==_name) {
                target.KeybindArr.emplace_back();
                ok &= loadFrom(s,target.KeybindArr.back());
            } else
                assert(!"unknown field referenced.");
            s->nest_out();
        }

        target.KeybindArr = correctSecondaryBinds(target.KeybindArr);

        sCDebug(logKeybinds) << "Total Keybinds:" << target.KeybindArr.size();

        assert(ok);
        return ok;
    }

    bool loadFrom(BinStore * s, Command & target)
    {
        s->prepare();
        bool ok = true;
        ok &= s->read(target.CmdString);
        ok &= s->read(target.DisplayName);
        ok &= s->prepare_nested(); // will update the file size left
        if(s->end_encountered())
            return ok;

        sCDebug(logKeybinds) << target.CmdString << target.DisplayName;

        assert(ok);
        return ok;
    }

    bool loadFrom(BinStore * s, CommandCategory_Entry & target)
    {
        s->prepare();
        bool ok = true;
        ok &= s->read(target.DisplayName);
        ok &= s->prepare_nested(); // will update the file size left
        if(s->end_encountered())
            return ok;
        String _name;
        while(s->nesting_name(_name))
        {
            s->nest_in();
            if("Command"==_name) {
                target.commands.emplace_back();
                ok &= loadFrom(s,target.commands.back());
            } else
                assert(!"unknown field referenced.");
            s->nest_out();
        }
        assert(ok);
        return ok;
    }
} // namespace

bool loadFrom(BinStore * s, Parse_AllKeyProfiles & target)
{
    s->prepare();
    bool ok = true;
    ok &= s->prepare_nested(); // will update the file size left
    if(s->end_encountered())
        return ok;
    String _name;
    while(s->nesting_name(_name))
    {
        s->nest_in();
        if("KeyProfile"==_name) {
            target.emplace_back();
            ok &= loadFrom(s,target.back());
        } else
            assert(!"unknown field referenced.");
        s->nest_out();
    }
    assert(ok);
    return ok;
}

bool loadFrom(BinStore * s, Parse_AllCommandCategories & target)
{
    s->prepare();
    bool ok = true;
    ok &= s->prepare_nested(); // will update the file size left
    if(s->end_encountered())
        return ok;
    String _name;
    while(s->nesting_name(_name))
    {
        s->nest_in();
        if("CommandCategory"==_name) {
            target.emplace_back();
            ok &= loadFrom(s,target.back());
        } else
            assert(!"unknown field referenced.");
        s->nest_out();
    }
    assert(ok);
    return ok;
}

const constexpr uint32_t KeybindSettings::class_version;
CEREAL_CLASS_VERSION(KeybindSettings, KeybindSettings::class_version) // register Keybinds class version

template<class Archive>
void serialize(Archive &archive, Keybind &k)
{
    archive(cereal::make_nvp("Key",k.Key));
    archive(cereal::make_nvp("Mods",k.Mods));
    archive(cereal::make_nvp("KeyString",k.KeyString));
    archive(cereal::make_nvp("Command",k.Command));
    archive(cereal::make_nvp("IsSecondary",k.IsSecondary));
}

template<class Archive>
void serialize(Archive &archive, Keybind_Profiles &kp)
{
    archive(cereal::make_nvp("DisplayName",kp.DisplayName));
    archive(cereal::make_nvp("Name",kp.Name));
    archive(cereal::make_nvp("KeybindArr",kp.KeybindArr));
}

template<class Archive>
void serialize(Archive &archive, KeybindSettings &kbds, const uint32_t version)
{
    if(version != KeybindSettings::class_version)
    {
        sCritical() << "Failed to serialize KeybindSettings, incompatible serialization format version " << version;
        return;
    }

    archive(cereal::make_nvp("AllProfiles",kbds.m_keybind_profiles));
    archive(cereal::make_nvp("SelectedProfile",kbds.m_cur_keybind_profile));
}

template<class Archive>
void serialize(Archive &archive, CommandEntry &k, const uint32_t /*version*/)
{
    archive(cereal::make_nvp("Key",k.Key));
    archive(cereal::make_nvp("Mods",k.Mods));
}

template<class Archive>
void serialize(Archive &archive, Command &k, const uint32_t /*version*/)
{
    archive(cereal::make_nvp("CommandString",k.CmdString));
    archive(cereal::make_nvp("DisplayName",k.DisplayName));
    archive(cereal::make_nvp("CommandArr",k.CommandArr));
}

template<class Archive>
void serialize(Archive &archive, CommandCategory_Entry &k, const uint32_t /*version*/)
{
    archive(cereal::make_nvp("DisplayName",k.DisplayName));
    archive(cereal::make_nvp("Commands",k.commands));
}

template<class Archive>
void serialize(Archive &archive, Parse_AllCommandCategories &k, const uint32_t /*version*/)
{
    archive(cereal::make_nvp("AllCommandCategories",k));
}

void saveTo(const KeybindSettings &target, const String &baseName, bool text_format)
{
    SEGS::commonSaveTo(target,"KeybindSettings",baseName,text_format);
}

SPECIALIZE_VERSIONED_SERIALIZATIONS(KeybindSettings)

void serializeToDb(const KeybindSettings &data, String &tgt)
{
    std::ostringstream ostr;
    {
        cereal::JSONOutputArchive ar(ostr);
        ar(data);
    }
    tgt = String(ostr.str().c_str());
}

void serializeFromDb(KeybindSettings &data,const String &src)
{
    if(src.empty())
        return;
    std::istringstream istr;
    istr.str(src.c_str());
    {
        cereal::JSONInputArchive ar(istr);
        ar(data);
    }
}

//! @}
