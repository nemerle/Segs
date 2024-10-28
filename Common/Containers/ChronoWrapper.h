#pragma once

#include <compare>
#include <cstdint>

// sadly std::chrono includes a bunch of headers ( vector/algorithm etc ), so we wrap it here
struct ChronoWrapper
{
    ChronoWrapper();
    static ChronoWrapper now();

    // chrono object will be inplace constructed here
    uint8_t data[16];
};
std::strong_ordering operator<=>(const ChronoWrapper &lhs, const ChronoWrapper &rhs);

template<typename T>
T toChronoType(ChronoWrapper val);

template<typename T>
ChronoWrapper fromChronoType(T val);


struct SteadyChronoWrapper
{
    SteadyChronoWrapper();
    static SteadyChronoWrapper now();

           // chrono object will be inplace constructed here
    uint8_t data[16];
};
std::strong_ordering operator<=>(const SteadyChronoWrapper &lhs, const SteadyChronoWrapper &rhs);

template<typename T>
T toSteadyChronoType(SteadyChronoWrapper val);

template<typename T>
SteadyChronoWrapper fromSteadyChronoType(T val);
