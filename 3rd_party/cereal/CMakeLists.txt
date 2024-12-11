cmake_minimum_required(VERSION 3.16)

project (cereal)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(SKIP_PORTABILITY_TEST "Skip portability tests" OFF)
if(NOT CMAKE_VERSION VERSION_LESS 3.0) # installing cereal requires INTERFACE lib
    option(JUST_INSTALL_CEREAL "Don't do anything besides installing the library" OFF)
endif()

if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "-Wall -Werror -g -Wextra -Wshadow -pedantic ${CMAKE_CXX_FLAGS}")
endif()

add_library(cereal INTERFACE)
target_include_directories(cereal INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
install(TARGETS cereal EXPORT cereal
    DESTINATION lib) # ignored
install(EXPORT cereal FILE cereal-config.cmake
    DESTINATION share/cmake/cereal)
install(DIRECTORY include/cereal DESTINATION include)

return()
