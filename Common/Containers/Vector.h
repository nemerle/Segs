#pragma once

#include "EASTL/vector.h"
#include "EASTL/fixed_vector.h"
#include "EASTL/span.h"

template <class T>
using Vector = eastl::vector<T>;

template<class T,int N,bool GROWING>
using FixedVector = eastl::fixed_vector<T,N,GROWING>;

template <typename T,size_t sz = eastl::dynamic_extent>
using Span = eastl::span<T,sz>;
