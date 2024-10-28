#pragma once

#include "EASTL/unordered_set.h"
template <class TKey, class HashFunc = eastl::hash<TKey>, class CompareFunc = eastl::equal_to<TKey>>
using HashSet = eastl::unordered_set<TKey, HashFunc, CompareFunc>;