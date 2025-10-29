set(SegsUtils_SRC
    string_utils.cpp
    string_utils.h
    ServiceLocator.cpp
    IServiceLocator.h
    IFilesystem.h
    FilesystemHandler.cpp
    FilesystemHandler.h
    PiggFsHandler.cpp
    PiggFsHandler.h
    IFile.h
)

add_library(SegsUtils STATIC ${SegsUtils_SRC})

target_include_directories(SegsUtils PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_link_libraries(SegsUtils PUBLIC SegsContainers)
set_property(TARGET SegsUtils PROPERTY POSITION_INDEPENDENT_CODE ON)
