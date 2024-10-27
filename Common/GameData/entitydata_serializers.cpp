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

#include "entitydata_serializers.h"

#include "entitydata_definitions.h"
#include "DataStorage.h"
#include "Components/serialization_common.h"
#include "Components/serialization_types.h"

const constexpr uint32_t EntityData::class_version;
CEREAL_CLASS_VERSION(EntityData, EntityData::class_version) // register EntityData class version

template<class Archive>
void serialize(Archive & archive, EntityData &ed, uint32_t const version)
{
    if(version > EntityData::class_version || version < 2)
    {
        sCritical() << "Failed to serialize EntityData, incompatible serialization format version " << version;
        return;
    }

    archive(cereal::make_nvp("AccessLevel",ed.m_access_level));
    archive(cereal::make_nvp("OriginIdx",ed.m_origin_idx));
    archive(cereal::make_nvp("ClassIdx",ed.m_class_idx));
    archive(cereal::make_nvp("Position",ed.m_pos));
    archive(cereal::make_nvp("Orientation",ed.m_orientation_pyr));

    if(version >= 3)
        archive(cereal::make_nvp("MapIdx",ed.m_map_idx));
}

void saveTo(const EntityData & target, const String & baseName, bool text_format)
{
    SEGS::commonSaveTo(target,"EntityData",baseName,text_format);
}

template
void serialize<cereal::JSONOutputArchive>(cereal::JSONOutputArchive & archive, EntityData & m, uint32_t const version);
template
void serialize<cereal::JSONInputArchive>(cereal::JSONInputArchive & archive, EntityData & m, uint32_t const version);

void serializeToDb(const EntityData &data, String &tgt)
{
    std::ostringstream ostr;
    {
        cereal::JSONOutputArchive ar(ostr);
        ar(data);
    }
    tgt = String(ostr.str().c_str());
}

void serializeFromDb(EntityData &data,const String &src)
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
