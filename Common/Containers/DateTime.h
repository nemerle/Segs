/*
 * SEGS - Super Entity Game Server
 * http://www.segs.dev/
 * Copyright (c) 2006 - 2024 SEGS Team (see AUTHORS.md)
 * This software is licensed under the terms of the 3-clause BSD License. See LICENSE.md for details.
 */
#pragma once

#include "Containers/ChronoWrapper.h"

class DateTime {
public:
    // Default constructor, initializes to the current system time
    DateTime() : timePoint() {}
    DateTime(ChronoWrapper tp) : timePoint(tp) {}
    DateTime(int year, uint8_t month, uint8_t day);

    static DateTime fromMSecsSinceEpoch(int64_t ms_since_epoch);

    static DateTime now();
    // Converts DateTime to milliseconds since the epoch (QDateTime::toMSecsSinceEpoch equivalent)
    int64_t toMSecsSinceEpoch() const;

    int64_t secsTo(const DateTime &other) const;

    // Conversion operator to std::chrono::time_point<std::chrono::system_clock>
    operator ChronoWrapper() const {
        return timePoint;
    }

    bool isValid() const;
    // Comparison operators using C++20 features
    auto operator<=>(const DateTime &other) const = default;
    auto operator<=>(const ChronoWrapper &other) const { return timePoint <=> other; }

private:
    ChronoWrapper timePoint;
};

// Non-member comparison operators for std::chrono::system_clock::time_point on the left side
std::strong_ordering operator<=>(const ChronoWrapper &lhs, const DateTime &rhs);
