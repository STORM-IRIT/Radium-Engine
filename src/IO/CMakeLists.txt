set( RADIUM_IO_IS_INTERFACE TRUE )

set(io_target radiumIO)

file(GLOB io_sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
                     ${CMAKE_CURRENT_SOURCE_DIR}/deprecated/*.cpp)
file(GLOB io_headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h
                     ${CMAKE_CURRENT_SOURCE_DIR}/deprecated/*.h
                     ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
                     ${CMAKE_CURRENT_SOURCE_DIR}/deprecated/*.hpp)
file(GLOB io_inlines ${CMAKE_CURRENT_SOURCE_DIR}/*.inl
                     ${CMAKE_CURRENT_SOURCE_DIR}/deprecated/*.inl)

set(io_libs radiumCore radiumEngine)

if( RADIUM_CAMERA_LOADER_SUPPORT )
    add_subdirectory( CameraLoader )
    set( io_sources ${io_sources} ${cameraloader_sources} )
    set( io_headers ${io_headers} ${cameraloader_headers} )
    set( io_inlines ${io_inlines} ${cameraloader_inlines} )
endif( RADIUM_CAMERA_LOADER_SUPPORT )

if( RADIUM_ASSIMP_SUPPORT )
    add_subdirectory( AssimpLoader )
    set( io_sources ${io_sources} ${assimp_sources} )
    set( io_headers ${io_headers} ${assimp_headers} )
    set( io_inlines ${io_inlines} ${assimp_inlines} )
    set( io_libs    ${io_libs}    ${ASSIMP_LIBRARIES} )
endif( RADIUM_ASSIMP_SUPPORT )

if( RADIUM_TINYPLY_SUPPORT )
    add_subdirectory( TinyPlyLoader )
    set( io_sources ${io_sources} ${tinyply_sources} )
    set( io_headers ${io_headers} ${tinyply_headers} )
    set( io_inlines ${io_inlines} ${tinyply_inlines} )
endif( RADIUM_TINYPLY_SUPPORT )

if ( RADIUM_IO_IS_INTERFACE )
    add_library( ${io_target} INTERFACE )

    target_sources(
        ${io_target} INTERFACE
        ${io_sources}
        ${io_headers}
        ${io_inlines}
    )
    target_compile_definitions(${io_target} INTERFACE "-DRA_IO_STATIC")

    target_link_libraries(
            ${io_target} INTERFACE
            PUBLIC ${io_libs}
    )
else ( RADIUM_IO_IS_INTERFACE )
    add_library(
        ${io_target} SHARED
        ${io_sources}
        ${io_headers}
        ${io_inlines}
    )
    target_compile_definitions(${io_target} PRIVATE "-DRA_IO_EXPORTS")

    target_link_libraries(
        ${io_target}
        PUBLIC ${io_libs}
    )
    set_property( TARGET ${io_target} PROPERTY POSITION_INDEPENDENT_CODE ON )

    if (NOT APPLE)
        radium_cotire(${io_target}) # see https://travis-ci.org/STORM-IRIT/Radium-Engine/jobs/432345198
    endif (NOT APPLE)
endif ( RADIUM_IO_IS_INTERFACE )


add_dependencies( ${io_target} radiumCore radiumEngine )
target_include_directories(${io_target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/.. )

if( RADIUM_ASSIMP_SUPPORT )
    add_dependencies( ${io_target} assimp_lib )
    target_include_directories( ${io_target}  PRIVATE ${ASSIMP_INCLUDE_DIR} )
endif( RADIUM_ASSIMP_SUPPORT )

if( RADIUM_TINYPLY_SUPPORT )
    add_dependencies( ${io_target} tinyply )
endif( RADIUM_TINYPLY_SUPPORT )

