/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2019 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

#pragma once

#include "Common/Containers/String.h"
#include "Containers/Vector.h"

class PasswordHasher
{
public:
    PasswordHasher();
    String          generateSalt();
    Vector<uint8_t> hashPassword(const String &pass, const String &salt);

protected:
    String getRandomString(int length) const;

    //QCryptographicHash m_hasher;
};
