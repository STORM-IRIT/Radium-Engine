project(PBD_lib)

cmake_minimum_required(VERSION 2.8.11)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../CMakeModules)

find_package( Qt5Core    REQUIRED )
find_package( Qt5Widgets REQUIRED )


# use Radium's common compilation options
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${RADIUM_ROOT}/cmake)
set(RADIUM_WITH_OMP ON)

if ( NOT RADIUM_PLUGIN_OUTPUT_PATH )
  set ( RADIUM_PLUGIN_OUTPUT_PATH "${RADIUM_ROOT}/${CMAKE_BUILD_TYPE}/Plugins")
endif()
message( "${PROJECT_NAME} output path : ${RADIUM_PLUGIN_OUTPUT_PATH}" )
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${RADIUM_PLUGIN_OUTPUT_PATH} )

if ( NOT CMAKE_PREFIX_PATH )
  set( CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR} )
endif()

# Win32 stuff
if (MSVC OR MSVC_IDE)
    # Copy libs / targets in the correct directories
    if ("${CMAKE_GENERATOR}" STREQUAL "NMake Makefiles")
        set(PDB_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
    else()
        foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
            string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
            set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${RADIUM_PLUGIN_OUTPUT_PATH})
            set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${RADIUM_PLUGIN_OUTPUT_PATH})
            set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${RADIUM_PLUGIN_OUTPUT_PATH})
        endforeach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    endif()
endif(MSVC OR MSVC_IDE)


file( GLOB_RECURSE sources *.cpp )
file( GLOB_RECURSE headers *.hpp )
file( GLOB_RECURSE inlines *.inl )


set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_definitions( ${Qt5Core_DEFINITIONS} )
add_definitions( ${Qt5Widgets_DEFINITIONS} )

include_directories(
    src/
    ${CMAKE_CURRENT_BINARY_DIR} # MOC
    ${RADIUM_INCLUDE_DIRS}
    )

# Our library project uses these sources and headers.
add_library(
    PBD_lib MODULE
    ${sources}
    ${headers}
    ${inlines}
    ${app_uis})



