# include only library
add_library(SegsContainers STATIC
    Vector.h HashMap.h String.h StringView.h Deque.h List.h
    ChronoWrapper.cpp ChronoWrapper.h
    DateTime.cpp  DateTime.h
)
target_include_directories(SegsContainers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../..)
target_link_libraries(SegsContainers PUBLIC EASTL)
