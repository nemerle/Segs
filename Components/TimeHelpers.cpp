/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2018 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */

/*!
 * @addtogroup Components Components
 * @{
 */

#include "TimeHelpers.h"
#include <chrono>

int64_t getSecsSince2000Epoch()
{
    using namespace std::chrono;

    // Define the epoch (January 1, 2000 00:00:00 UTC)
    const auto epoch = sys_days{January/1/2000};

    // Get the current time
    const auto now = system_clock::now();

    // Calculate the duration since the epoch
    const auto duration = now - epoch;

    // Convert the duration to seconds
    return duration_cast<seconds>(duration).count();
}