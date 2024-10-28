# standalone environment uses Qt as a platform abstraction provider

find_package(Qt6 COMPONENTS Core REQUIRED)

add_subdirectory(Services)

add_library(SEGS_Env INTERFACE)
target_link_libraries(SEGS_Env INTERFACE StandaloneServices)
