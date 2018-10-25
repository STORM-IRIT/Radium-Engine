project(PositionBasedSkinning)

cmake_minimum_required(VERSION 2.8.11)
add_subdirectory(libPBD)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../CMakeModules)


if (APPLE)
    cmake_policy(SET CMP0042 NEW)
    # RPATH settings on macOS do not affect install_name.
    # Run "cmake --help-policy CMP0068" for policy details.
    cmake_policy(SET CMP0068 NEW)
endif(APPLE)

message( "${PROJECT_NAME} output path : ${RADIUM_PLUGIN_OUTPUT_PATH}" )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${RADIUM_PLUGIN_OUTPUT_PATH} )
set(EXECUTABLE_OUTPUT_PATH         ${RADIUM_PLUGIN_OUTPUT_PATH} )
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${RADIUM_PLUGIN_OUTPUT_PATH} )
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${RADIUM_PLUGIN_OUTPUT_PATH} )

if ( NOT CMAKE_PREFIX_PATH )
  set( CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR} )
endif()

# Win32 stuff
if (MSVC OR MSVC_IDE)
    # Copy libs / targets in the correct directories
    if ("${CMAKE_GENERATOR}" STREQUAL "NMake Makefiles")
        set(PDB_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    else()
        foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
            string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
            set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
            set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
            set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
        endforeach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    endif()
endif(MSVC OR MSVC_IDE)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

file( GLOB_RECURSE sources src/*.cpp )
file( GLOB_RECURSE headers src/*.hpp )
file( GLOB_RECURSE inlines src/*.inl )

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_definitions( ${Qt5Core_DEFINITIONS} )
add_definitions( ${Qt5Widgets_DEFINITIONS} )

include_directories(
    src/
    ${CMAKE_CURRENT_BINARY_DIR} # MOC
    ${RADIUM_INCLUDE_DIRS}
    )

# Our library project uses these sources and headers.
add_library(
    PositionBasedSkinning MODULE
    ${sources}
    ${headers}
    ${inlines}
    ${app_uis})

target_compile_definitions(PositionBasedSkinning PRIVATE "-DPositionBasedSkinning_EXPORTS")

target_link_libraries(
                   PositionBasedSkinning
                   ${RA_CORE_LIB}
                   ${RA_ENGINE_LIB}
                   ${RA_GUIBASE_LIB}
                   ${Qt5Core_LIBRARIES}
                   ${Qt5Widgets_LIBRARIES}
                )

