add_library(StandaloneServices STATIC
    StandaloneServices.h StandaloneServices.cpp)

target_include_directories(StandaloneServices PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(StandaloneServices PUBLIC EASTL SegsUtils)
target_link_libraries(StandaloneServices PRIVATE Qt6::Core)
