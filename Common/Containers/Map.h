#pragma once

#include "EASTL/map.h"

template <class K,class V, class C = eastl::less<K>>
using Map = eastl::map<K,V,C>;
