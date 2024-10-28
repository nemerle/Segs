#include "DateTime.h"

#include <chrono>

using ChronoData = std::chrono::system_clock::time_point;


DateTime::DateTime(int year, uint8_t month, uint8_t day)
{
    std::tm tm = {};
    tm.tm_year = year - 1900; // Years since 1900
    tm.tm_mon  = month - 1;   // Months are 0-11
    tm.tm_mday = day;
    tm.tm_hour = 0;
    tm.tm_min  = 0;
    tm.tm_sec  = 0;

    std::time_t time = std::mktime(&tm);
    if (time != -1)
    {
        timePoint = fromChronoType(std::chrono::system_clock::from_time_t(time));
    }
    else
    {
        // Handle invalid date by setting to max time point
        timePoint = fromChronoType(std::chrono::system_clock::time_point::max());
    }
}

DateTime DateTime::fromMSecsSinceEpoch(int64_t ms_since_epoch)
{
    // Convert milliseconds into a duration
    std::chrono::milliseconds duration(ms_since_epoch);
    return {fromChronoType(std::chrono::system_clock::time_point(duration))};
}

DateTime DateTime::now()
{
    return {fromChronoType(std::chrono::system_clock::now())};
}

int64_t DateTime::toMSecsSinceEpoch() const
{
    auto duration = toChronoType<ChronoData>(timePoint).time_since_epoch();
    auto millis   = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return static_cast<int64_t>(millis);
}

int64_t DateTime::secsTo(const DateTime &other) const
{
    auto duration = toChronoType<ChronoData>(other.timePoint) - toChronoType<ChronoData>(timePoint);
    return std::chrono::duration_cast<std::chrono::seconds>(duration).count();
}

bool DateTime::isValid() const
{
    return toChronoType<ChronoData>(timePoint) != ChronoData::max();
}

std::strong_ordering operator<=>(const ChronoWrapper &lhs, const DateTime &rhs)
{
    return lhs <=> static_cast<ChronoWrapper>(rhs);
}
