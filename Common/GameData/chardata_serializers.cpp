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

#include "chardata_serializers.h"
#include "GameData/CharacterData.h"
#include "Components/serialization_common.h"
#include "Components/serialization_types.h"
#include "GameData/Powers.h"

#include "DataStorage.h"
#include "trays_serializers.h"
#include "attrib_serializers.h"

#include "Components/Logging.h"


void saveTo(const CharacterData &target, const String &baseName, bool text_format)
{
    SEGS::commonSaveTo(target,"CharacterData",baseName,text_format);
}


//! @}
