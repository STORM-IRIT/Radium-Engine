set(library_version "0.1.0")

set( CMAKE_SKIP_BUILD_RPATH FALSE )
set( CMAKE_BUILD_WITH_INSTALL_RPATH FALSE )
set( CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib )
set( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE )

set( RADIUM_IO_IS_INTERFACE TRUE )

list( FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSysDir )
if ( "${isSysDir}" STREQUAL "-1" )
    set( CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib" )
endif( "${isSysDir}" STREQUAL "-1" )

# Find packages
find_package(OpenGL     REQUIRED)
find_package(Qt5Core    REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL  REQUIRED)


include_directories(
    ${EIGEN3_INCLUDE_DIR}
    ${GLBINDING_INCLUDE_DIR}
    ${GLOBJECTS_INCLUDE_DIR}
    ${GLM_INCLUDE_DIR}
)

message(STATUS "Compiling Radium with include dirs : ${EIGEN3_INCLUDE_DIR} -- ${GLBINDING_INCLUDE_DIR} -- ${GLM_INCLUDE_DIR} -- ${GLOBJECTS_INCLUDE_DIR}" )

if( RADIUM_ASSIMP_SUPPORT )
    include_directories( ${ASSIMP_INCLUDE_DIR} )
    message(STATUS "Compiling Radium with Assimp's include dir : ${ASSIMP_INCLUDE_DIR}" )
endif( RADIUM_ASSIMP_SUPPORT )

# Build Core
set(core_target radiumCore)

include(CMakeGenerateVersion.txt)

file(GLOB_RECURSE core_sources Core/*.cpp)
file(GLOB_RECURSE core_headers Core/*.h Core/*.hpp)
file(GLOB_RECURSE core_inlines Core/*.inl)

add_library(
    ${core_target} SHARED
    ${core_sources}
    ${core_headers}
    ${core_inlines}
)

add_dependencies( ${core_target} eigen3)
target_compile_definitions(${core_target} PRIVATE "-DRA_CORE_EXPORTS")

target_link_libraries(
    ${core_target}
)

# Build IO
set(io_target radiumIO)

file(GLOB io_sources IO/*.cpp)
file(GLOB io_headers IO/*.h IO/*.hpp)
file(GLOB io_inlines IO/*.inl)

if( RADIUM_ASSIMP_SUPPORT )
    add_subdirectory( IO/AssimpLoader )
endif( RADIUM_ASSIMP_SUPPORT )

set(io_libs
    radiumCore
    ${PBRT_LIBRARIES}
)

if( RADIUM_ASSIMP_SUPPORT )
    set( io_libs ${ASSIMP_LIBRARIES} ${io_libs} )

    set( io_sources ${assimp_sources} ${io_sources} )
    set( io_headers ${assimp_headers} ${io_headers} )
    set( io_inlines ${assimp_inlines} ${io_inlines} )
endif( RADIUM_ASSIMP_SUPPORT )

if( RADIUM_IO_IS_INTERFACE )
    add_library(
            ${io_target} INTERFACE
    )

    target_sources(
            ${io_target} INTERFACE
            ${io_sources}
            ${io_headers}
            ${io_inlines}
    )
else( RADIUM_IO_IS_INTERFACE )
    add_library(
            ${io_target} SHARED
            ${io_sources}
            ${io_headers}
            ${io_inlines}
    )
endif( RADIUM_IO_IS_INTERFACE )

add_dependencies( ${io_target} ${core_target} )#pbrt_lib)

if( RADIUM_ASSIMP_SUPPORT )
    add_dependencies( ${io_target} assimp_lib ${io_target} )
endif( RADIUM_ASSIMP_SUPPORT )

if( RADIUM_IO_IS_INTERFACE )
    target_compile_definitions(${io_target} INTERFACE "-DRA_IO_EXPORTS")

    target_link_libraries(
            ${io_target} INTERFACE
            ${io_libs}
    )
else( RADIUM_IO_IS_INTERFACE )
    target_compile_definitions(${io_target} PRIVATE "-DRA_IO_EXPORTS")

    target_link_libraries(
            ${io_target}
            ${io_libs}
    )
endif( RADIUM_IO_IS_INTERFACE )

# Build Engine
set(engine_target radiumEngine)

file(GLOB_RECURSE engine_sources Engine/*.cpp)
file(GLOB_RECURSE engine_headers Engine/*.h Engine/*.hpp)
file(GLOB_RECURSE engine_inlines Engine/*.inl)

file(GLOB_RECURSE engine_glsl ../Shaders/*.glsl)

set( engine_shaders ${engine_glsl})

set(engine_libs
    radiumCore
    ${Qt5OpenGL_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${GLBINDING_LIBRARIES}
    ${GLOBJECTS_LIBRARIES}
    ${GLM_LIBRARIES}
   )

if( RADIUM_ASSIMP_SUPPORT )
    set( engine_libs ${ASSIMP_LIBRARIES} ${engine_libs} )
endif( RADIUM_ASSIMP_SUPPORT )

add_library(
    ${engine_target} SHARED
    ${engine_sources}
    ${engine_headers}
    ${engine_inlines}

    ${engine_shaders}
)

add_dependencies( ${engine_target} ${core_target} glbinding_lib globjects_lib glm_lib)

if( RADIUM_ASSIMP_SUPPORT )
    add_dependencies( ${engine_target} assimp_lib ${engine_target} )
endif( RADIUM_ASSIMP_SUPPORT )

target_compile_definitions(${engine_target} PRIVATE "-DRA_ENGINE_EXPORTS")

target_link_libraries(
    ${engine_target}
    ${engine_libs}
)


# Build GuiBase
set(guibase_target radiumGuiBase)

find_package(Qt5Xml REQUIRED)

# Qt5
set(Qt5_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5OpenGL_LIBRARIES} ${Qt5Xml_LIBRARIES})
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Fixme : Really usefull ?
add_definitions(${Qt5Core_DEFINITIONS})
add_definitions(${Qt5Widgets_DEFINITIONS})
add_definitions(${Qt5OpenGL_DEFINITIONS})
add_definitions(${Qt5Xml_DEFINITIONS})

file(GLOB_RECURSE guibase_sources GuiBase/*.cpp)
file(GLOB_RECURSE guibase_headers GuiBase/*.h GuiBase/*.hpp)
file(GLOB_RECURSE guibase_inlines GuiBase/*.inl)

set (guibase_libs
    radiumEngine
    radiumCore
    ${Qt5_LIBRARIES}
    ${GLBINDING_LIBRARIES}
    ${GLOBJECTS_LIBRARIES}
)

add_library(
    ${guibase_target} SHARED
    ${guibase_sources}
    ${guibase_headers}
    ${guibase_inlines})

add_dependencies( ${guibase_target} ${engine_target} )
target_compile_definitions(${guibase_target} PRIVATE "-DRA_GUIBASE_EXPORTS")

target_link_libraries(
    ${guibase_target}
    ${guibase_libs}
)

# Build tests
include_directories(
        .
)
add_subdirectory(Tests)
