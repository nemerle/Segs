#include "EASTL/allocator.h"
#include <stdlib.h>
// if we're linking EASTL as static library, we need to provide the required new operators
// in the future we could add MemoryService to ServiceLocator to integrate allocations with another system.
void* operator new[](size_t size, const char* pName, int flags, unsigned debugFlags, const char* file, int line) {
    return new char[size];
}
void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line) {
    return new char[size]; //std::aligned_alloc(alignment,size);
}

