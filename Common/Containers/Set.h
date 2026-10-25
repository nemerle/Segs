#pragma once

#include "EASTL/set.h"
template <class TKey, class CompareFunc = eastl::less<TKey>>
using Set = eastl::set<TKey, CompareFunc>;
