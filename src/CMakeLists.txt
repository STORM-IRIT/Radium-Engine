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
    ${OPENMESH_INCLUDE_DIR}
)

message(STATUS "Compiling Radium with 3rdparty include dirs : ${EIGEN3_INCLUDE_DIR} -- ${GLBINDING_INCLUDE_DIR} -- ${GLM_INCLUDE_DIR} -- ${GLOBJECTS_INCLUDE_DIR}-- ${OPENMESH_INCLUDE_DIR}" )

if( RADIUM_ASSIMP_SUPPORT )
        set( RADIUM_IO_IS_INTERFACE FALSE )
    include_directories( ${ASSIMP_INCLUDE_DIR} )
    message(STATUS "Compiling Radium with Assimp's include dir : ${ASSIMP_INCLUDE_DIR}" )
endif( RADIUM_ASSIMP_SUPPORT )

# Build Core
set(core_target radiumCore)

set(RADIUM_VERSION_FILE ${CMAKE_CURRENT_BINARY_DIR}/Version.cpp)
add_custom_command(
    OUTPUT ${RADIUM_VERSION_FILE}
    COMMAND ${CMAKE_COMMAND}
                             -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                             -DCMAKE_CXX_COMPILER_VERSION=${CMAKE_CXX_COMPILER_VERSION}
                             -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
                             -DCMAKE_CXX_FLAGS_DEBUG=${CMAKE_CXX_FLAGS_DEBUG}
                             -DCMAKE_CXX_FLAGS_RELEASE=${CMAKE_CXX_FLAGS_RELEASE}
                             -DGIT_CHANGESET=${GIT_CHANGESET}
                             -P ${CMAKE_CURRENT_SOURCE_DIR}/CMakeGenerateVersion.txt
    COMMENT "Generate Radium Version file"
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/CMakeGenerateVersion.txt
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    VERBATIM
    )
add_custom_target(radiumVersionFile DEPENDS ${RADIUM_VERSION_FILE})

file(GLOB_RECURSE core_sources Core/*.cpp)
file(GLOB_RECURSE core_headers Core/*.h Core/*.hpp)
file(GLOB_RECURSE core_inlines Core/*.inl)

set(core_libs
    ${OPENMESH_LIBRARIES}
   )

add_library(
    ${core_target} SHARED
    ${core_sources}
    ${core_headers}
    ${core_inlines}
    ${RADIUM_VERSION_FILE} #This file is build-dependant and generated in the Bundle
)

add_dependencies( ${core_target} eigen3 radiumVersionFile openmesh_lib)
target_compile_definitions(${core_target} PRIVATE "-DRA_CORE_EXPORTS")

target_link_libraries(
    ${core_target}
    ${core_libs}
)

# Build IO
set(io_target radiumIO)

file(GLOB io_sources IO/*.cpp)
file(GLOB io_headers IO/*.h IO/*.hpp)
file(GLOB io_inlines IO/*.inl)

set(io_libs radiumCore radiumEngine)

if( RADIUM_ASSIMP_SUPPORT )
    add_subdirectory( IO/AssimpLoader )
    set( io_sources ${io_sources} ${assimp_sources} )
    set( io_headers ${io_headers} ${assimp_headers} )
    set( io_inlines ${io_inlines} ${assimp_inlines} )
    set( io_libs    ${io_libs}    ${ASSIMP_LIBRARIES} )
endif( RADIUM_ASSIMP_SUPPORT )

if( RADIUM_TINYPLY_SUPPORT )
    add_subdirectory( IO/TinyPlyLoader )
    set( io_sources ${io_sources} ${tinyply_sources} )
    set( io_headers ${io_headers} ${tinyply_headers} )
    set( io_inlines ${io_inlines} ${tinyply_inlines} )
endif( RADIUM_TINYPLY_SUPPORT )

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
    target_compile_definitions(${io_target} INTERFACE "-DRA_IO_STATIC")

    target_link_libraries(
            ${io_target} INTERFACE
            ${io_libs}
    )
else( RADIUM_IO_IS_INTERFACE )
    add_library(
            ${io_target} SHARED
            ${io_sources}
            ${io_headers}
            ${io_inlines}
    )
    target_compile_definitions(${io_target} PRIVATE "-DRA_IO_EXPORTS")

    target_link_libraries(
            ${io_target}
            ${io_libs}
    )
    set_property( TARGET ${io_target} PROPERTY POSITION_INDEPENDENT_CODE ON )
endif( RADIUM_IO_IS_INTERFACE )

add_dependencies( ${io_target} ${core_target} ${engine_target} )

if( RADIUM_ASSIMP_SUPPORT )
    add_dependencies( ${io_target} assimp_lib )
endif( RADIUM_ASSIMP_SUPPORT )

if( RADIUM_TINYPLY_SUPPORT )
    add_dependencies( ${io_target} tinyply )
endif( RADIUM_TINYPLY_SUPPORT )

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

add_library(
    ${engine_target} SHARED
    ${engine_sources}
    ${engine_headers}
    ${engine_inlines}
    ${engine_shaders}
)

add_dependencies( ${engine_target} ${core_target} glbinding_lib globjects_lib glm_lib)

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
    ${OPENGL_LIBRARIES}
    ${GLBINDING_LIBRARIES}
    ${GLOBJECTS_LIBRARIES}
    ${GLM_LIBRARIES}
)

add_library(
    ${guibase_target} SHARED
    ${guibase_sources}
    ${guibase_headers}
    ${guibase_inlines})

add_dependencies( ${guibase_target} ${engine_target} glbinding_lib globjects_lib)
target_compile_definitions(${guibase_target} PRIVATE "-DRA_GUIBASE_EXPORTS")

target_link_libraries(
    ${guibase_target}
    ${guibase_libs}
    )


set(RADIUM_LIBRARIES ${RADIUM_LIBRARIES} "${core_target}" "${engine_target}" "${io_target}" "${guibase_target}" PARENT_SCOPE)

set_property( TARGET ${core_target} PROPERTY POSITION_INDEPENDENT_CODE ON )
set_property( TARGET ${engine_target} PROPERTY POSITION_INDEPENDENT_CODE ON )
set_property( TARGET ${guibase_target} PROPERTY POSITION_INDEPENDENT_CODE ON )

# Build tests
include_directories(
        .
)
add_subdirectory(Tests)
