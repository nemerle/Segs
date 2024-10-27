#pragma once

#include "EASTL/unordered_map.h"
template <class TKey, class TData, class HashFunc = eastl::hash<TKey>, class CompareFunc = eastl::equal_to<TKey>>
using HashMap = eastl::unordered_map<TKey, TData, HashFunc, CompareFunc>;