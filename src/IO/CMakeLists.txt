set(ra_io_target IO)

project( ${ra_io_target}
         LANGUAGES CXX
         VERSION ${RADIUM_VERSION})

option( RADIUM_IO_DEPRECATED  "Provide depricated loaders (to be removed without notice)" ON )
option( RADIUM_IO_ASSIMP  "Provide loaders based on Assimp library" OFF )
option( RADIUM_IO_TINYPLY "Provide loaders based on TinyPly library" ON )

include( filelistIO )

add_library(
    ${ra_io_target} SHARED
    ${io_sources}
    ${io_headers}
    ${io_inlines}
)

addExternalFolder( IO ${CMAKE_CURRENT_SOURCE_DIR}/external
    -DRADIUM_IO_ASSIMP=${RADIUM_IO_ASSIMP}
    -DRADIUM_IO_TINYPLY=${RADIUM_IO_TINYPLY} )

if( RADIUM_IO_ASSIMP )
    find_package(assimp 5.0 REQUIRED NO_DEFAULT_PATH )
    set_target_properties( ${ra_io_target} PROPERTIES IO_ASSIMP ${RADIUM_IO_ASSIMP})
endif( RADIUM_IO_ASSIMP )
if( RADIUM_IO_TINYPLY )
    find_package(tinyply REQUIRED NO_DEFAULT_PATH )
    set_target_properties( ${ra_io_target} PROPERTIES IO_TINYPLY ${RADIUM_IO_TINYPLY})
endif( RADIUM_IO_TINYPLY )

if (RADIUM_IO_ASSIMP)
    if (MSVC)
        if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
            get_target_property(assimplib assimp::assimp IMPORTED_IMPLIB_DEBUG)
        else ()
            get_target_property(assimplib assimp::assimp IMPORTED_IMPLIB_RELEASE)
        endif ()
        message(STATUS " ASSIMP LINK : ${assimplib}")
        target_link_libraries(${ra_io_target} PUBLIC ${assimplib})
    else ()
        target_link_libraries(${ra_io_target} PUBLIC assimp::assimp)
    endif ()

    set_target_properties(assimp::assimp PROPERTIES
        MAP_IMPORTED_CONFIG_MINSIZEREL Release
        MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release
        )
endif (RADIUM_IO_ASSIMP)
if (RADIUM_IO_TINYPLY)
    target_link_libraries(${ra_io_target} PUBLIC tinyply)
endif (RADIUM_IO_TINYPLY)

add_dependencies(${ra_io_target} Core)
target_link_libraries(${ra_io_target} PUBLIC Core)
target_compile_definitions(${ra_io_target} PRIVATE RA_IO_EXPORTS IO_USE_CAMERA_LOADER)

message(STATUS "Configuring library ${ra_io_target} with standard settings")
configureTargetWithDefaultRaSettings(${ra_io_target})
configure_radium_library(
    TARGET ${ra_io_target}
    FILES "${io_headers};${io_inlines}"
)
