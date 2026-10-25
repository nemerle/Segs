link_directories(${ThirdParty_Install_Dir}/lib)

include(ExternalProject_CEREAL.cmake)
include(ExternalProject_ACE.cmake)
if(MINGW OR MSVC) # ERICEDIT: If the OS is Windows:
    add_custom_target( # ERICEDIT: Add a target that depends on authserver.
        libace ALL
        DEPENDS ace_IMP
        )
    add_custom_command( # ERICEDIT: Copy over libACE.dll to the output directory.
        TARGET libace
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ace_LIBRARY_SHARED} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        )
endif()

add_library(glm INTERFACE)
target_include_directories(glm INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Recast/Detour (zlib licence, recastnavigation/VERSION): navigation mesh build and path queries.
file(GLOB SEGS_RECAST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/recastnavigation/Recast/Source/*.cpp)
add_library(segs_recast STATIC ${SEGS_RECAST_SOURCES})
target_include_directories(segs_recast PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/recastnavigation/Recast/Include)
file(GLOB SEGS_DETOUR_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/recastnavigation/Detour/Source/*.cpp)
add_library(segs_detour STATIC ${SEGS_DETOUR_SOURCES})
target_include_directories(segs_detour PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/recastnavigation/Detour/Include)
# 64-bit polygon references: a whole city zone has more tiles x polygons than 32-bit refs can address.
target_compile_definitions(segs_detour PUBLIC DT_POLYREF64=1 DT_VIRTUAL_QUERYFILTER=1)

########################################################################################
# Optional components
########################################################################################
include(ExternalProject_EASTL.cmake)
if(STANDALONE_ENV)
include(ExternalProject_LUA.cmake)
include(ExternalProject_SOL2.cmake)
include(ExternalProject_JCON.cmake)
endif()
