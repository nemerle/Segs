#pragma once

#include "EASTL/set.h"
template <class TKey, class CompareFunc = eastl::equal_to<TKey>>
using Set = eastl::set<TKey, CompareFunc>;
