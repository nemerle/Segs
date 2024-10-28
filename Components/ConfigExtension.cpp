/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

/*!
 * @addtogroup Components
 * @{
 */

#include "ConfigExtension.h"
#include "Common/Containers/String.h"
#include "Common/Containers/StringView.h"
#include "Common/Containers/Vector.h"
#include "Utils/string_utils.h"

#include <ace/INET_Addr.h>

bool parseAddress(const String &src,ACE_INET_Addr &tgt)
{
    // input is hostname:port
    FixedVector<StringView,2,true> parts;
    String trim=src.trimmed();
    String::split_ref(parts,trim,':');
    if(parts.size()!=2)
        return false;
    bool ok_port=false;
    int port=StringUtils::to_int(parts[1],&ok_port);
    if(!ok_port) {
        return false;
    }
    // set the ':' char to const char * terminating '\0' to allow us to use string view as a plain text pointer
    trim[parts[0].size()] = 0;
    tgt.set(port,parts[0].data());
    return true;
}

//! @}
