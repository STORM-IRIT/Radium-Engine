set(RA_IO_TARGET IO)
list(APPEND CMAKE_MESSAGE_INDENT "[${RA_IO_TARGET}] ")

project(${RA_IO_TARGET} LANGUAGES CXX VERSION ${RADIUM_VERSION})

option(RADIUM_IO_DEPRECATED "Provide deprecated loaders (to be removed without notice)" ON)
option(RADIUM_IO_ASSIMP "Provide loaders based on Assimp library" ON)
option(RADIUM_IO_TINYPLY "Provide loaders based on TinyPly library" ON)
option(RADIUM_IO_VOLUMES "Provide loader for volume pvm file format" ON)

include(filelist.cmake)

add_library(${RA_IO_TARGET} SHARED ${io_sources} ${io_headers} ${io_inlines})

# LOCAL_DEPENDENCIES form parent scope
populate_local_dependencies(NAME "assimp_DIR")
populate_local_dependencies(NAME "tinyply_DIR")

if(RADIUM_IO_ASSIMP)
    find_package(assimp 5.0 REQUIRED NO_DEFAULT_PATH)
    set_target_properties(${RA_IO_TARGET} PROPERTIES IO_HAS_ASSIMP ${RADIUM_IO_ASSIMP})
endif(RADIUM_IO_ASSIMP)
if(RADIUM_IO_TINYPLY)
    find_package(tinyply REQUIRED NO_DEFAULT_PATH)
    set_target_properties(${RA_IO_TARGET} PROPERTIES IO_HAS_TINYPLY ${RADIUM_IO_TINYPLY})
endif(RADIUM_IO_TINYPLY)

if(RADIUM_IO_ASSIMP)
    if(MSVC)
        if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
            get_target_property(assimplib assimp::assimp IMPORTED_IMPLIB_DEBUG)
        else()
            get_target_property(assimplib assimp::assimp IMPORTED_IMPLIB_RELEASE)
        endif()
        target_link_libraries(${RA_IO_TARGET} PUBLIC ${assimplib})
    else()
        target_link_libraries(${RA_IO_TARGET} PUBLIC assimp::assimp)
    endif()

    set_target_properties(
        assimp::assimp PROPERTIES MAP_IMPORTED_CONFIG_MINSIZEREL Release
                                  MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release
    )
endif(RADIUM_IO_ASSIMP)
if(RADIUM_IO_TINYPLY)
    target_link_libraries(${RA_IO_TARGET} PUBLIC tinyply)
endif(RADIUM_IO_TINYPLY)

if(RADIUM_IO_VOLUMES)
    set_target_properties(${RA_IO_TARGET} PROPERTIES IO_HAS_VOLUMES ${RADIUM_IO_VOLUMES})
endif()

add_dependencies(${RA_IO_TARGET} Core)
target_link_libraries(${RA_IO_TARGET} PUBLIC Core)

target_compile_options(${RA_IO_TARGET} PUBLIC ${RA_DEFAULT_COMPILE_OPTIONS})

target_compile_definitions(${RA_IO_TARGET} PRIVATE RA_IO_EXPORTS IO_USE_CAMERA_LOADER)

message(STATUS "Configuring library ${RA_IO_TARGET} with standard settings")
configure_radium_target(${RA_IO_TARGET})
configure_radium_library(
    TARGET ${RA_IO_TARGET} COMPONENT PACKAGE_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    FILES "${io_headers};${io_inlines}"
)
set(RADIUM_COMPONENTS ${RADIUM_COMPONENTS} ${RA_IO_TARGET} PARENT_SCOPE)

if(RADIUM_ENABLE_PCH)
    target_precompile_headers(${RA_IO_TARGET} PRIVATE pch.hpp)
endif()

list(REMOVE_AT CMAKE_MESSAGE_INDENT -1)
