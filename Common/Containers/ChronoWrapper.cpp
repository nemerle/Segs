#include "ChronoWrapper.h"

#include <chrono>

using ChronoData = std::chrono::system_clock::time_point;
using SteadyChronoData = std::chrono::steady_clock::time_point;
static_assert(sizeof(ChronoData)==8);

ChronoWrapper::ChronoWrapper() {
    new (data) ChronoData;
}

ChronoWrapper ChronoWrapper::now() {
    ChronoWrapper res;
    (ChronoData &)res.data = std::chrono::system_clock::now();
    return res;
}

std::strong_ordering operator<=>(const ChronoWrapper &lhs, const ChronoWrapper &rhs) {
    return (const ChronoData &)lhs.data <=> (const ChronoData &)rhs.data;
}


template <>
ChronoData toChronoType<ChronoData>(ChronoWrapper val) {
    return (const ChronoData &)val.data;
}

template <>
ChronoWrapper fromChronoType<ChronoData>(ChronoData val) {
    ChronoWrapper res;
    (ChronoData &)res.data = val;
    return res;
}


SteadyChronoWrapper::SteadyChronoWrapper() {
    new (data) ChronoData;
}

SteadyChronoWrapper SteadyChronoWrapper::now() {
    SteadyChronoWrapper res;
    (SteadyChronoData &)res.data = std::chrono::steady_clock::now();
    return res;
}

std::strong_ordering operator<=>(const SteadyChronoWrapper &lhs, const SteadyChronoWrapper &rhs) {
    return (const SteadyChronoData &)lhs.data <=> (const SteadyChronoData &)rhs.data;
}


template <>
SteadyChronoData toSteadyChronoType<SteadyChronoData>(SteadyChronoWrapper val) {
    return (const SteadyChronoData &)val.data;
}

template <>
SteadyChronoWrapper fromSteadyChronoType<SteadyChronoData>(SteadyChronoData val) {
    SteadyChronoWrapper res;
    (SteadyChronoData &)res.data = val;
    return res;
}
