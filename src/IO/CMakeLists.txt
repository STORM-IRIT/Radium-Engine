set(ra_io_target RadiumIO)

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
add_library(Radium::IO ALIAS ${ra_io_target})

addExternalFolder( IO ${CMAKE_CURRENT_SOURCE_DIR}/external
    -DRADIUM_IO_ASSIMP=${RADIUM_IO_ASSIMP}
    -DRADIUM_IO_TINYPLY=${RADIUM_IO_TINYPLY} )

if( RADIUM_IO_ASSIMP )
    find_package(assimp 5.0 REQUIRED NO_DEFAULT_PATH )
endif( RADIUM_IO_ASSIMP )
if( RADIUM_IO_TINYPLY )
    find_package(tinyply REQUIRED NO_DEFAULT_PATH )
endif( RADIUM_IO_TINYPLY )

configureTargetWithDefaultRaSettings( ${ra_io_target} )
if( RADIUM_IO_ASSIMP )
    target_link_libraries (${ra_io_target} PUBLIC assimp::assimp)
endif( RADIUM_IO_ASSIMP )
if( RADIUM_IO_TINYPLY )
    target_link_libraries (${ra_io_target} PUBLIC tinyply)
endif( RADIUM_IO_TINYPLY )

configureTargetWithDefaultRaSettings( ${ra_io_target} )
add_dependencies( ${ra_io_target} Radium::Core )
target_link_libraries (${ra_io_target} PUBLIC Radium::Core)

target_compile_definitions(${ra_io_target} PRIVATE "-DRA_IO_EXPORTS")


# set_target_properties(${ra_io_target} PROPERTIES PUBLIC_HEADER "${io_headers};${io_inlines}")
install(TARGETS ${ra_io_target}
    EXPORT ${ra_io_target}Targets
    CONFIGURATIONS ${CMAKE_BUILD_TYPE}
    RUNTIME  DESTINATION bin
    OBJECTS  DESTINATION bin
    LIBRARY  DESTINATION lib
    ARCHIVE  DESTINATION lib
    INCLUDES DESTINATION include
    )

installLibHeaders( "IO" "${io_headers}" )
installLibHeaders( "IO" "${io_inlines}" )
