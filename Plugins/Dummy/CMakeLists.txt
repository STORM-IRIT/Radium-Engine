project(DummyPlugin)

cmake_minimum_required(VERSION 2.8.11)

# Debug by default !
if ( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Debug )
endif()

if ( NOT PLUGIN_OUTPUT_PATH )
  set ( PLUGIN_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib )
else()
  message( "Output path : ${PLUGIN_OUTPUT_PATH}" )
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_PATH} )
set(EXECUTABLE_OUTPUT_PATH         ${PLUGIN_OUTPUT_PATH} )
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_PATH} )
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_PATH} )

if ( NOT CMAKE_PREFIX_PATH )
  set( CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR} )
endif()

# Win32 stuff
if (MSVC OR MSVC_IDE)
  # Use November CTP 2013 (constexpr and other non implemented stuff in the 2013 version)
    if (MSVC_VERSION LESS 1800)
        message(FATAL_ERROR
                "This project requires C++11 stuff provided only with "
                "Microsoft Visual C++ Compiler Nov 2013 CTP (v120_CTP_Nov2013).")
    endif(MSVC_VERSION LESS 1800)

    if (MSVC_VERSION EQUAL 1800)
        #set(CMAKE_GENERATOR_TOOLSET "CTP_Nov2013" CACHE STRING "Platform Toolset" FORCE)
    endif (MSVC_VERSION EQUAL 1800)

    # Copy libs / targets in the correct directories
    if ("${CMAKE_GENERATOR}" STREQUAL "NMake Makefiles")
        set(PDB_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
    else()
        foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
            string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
            set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/bin)
            set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/lib)
            set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/lib)
        endforeach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    endif()
endif(MSVC OR MSVC_IDE)

find_package(Qt5Core    REQUIRED)
find_package(Qt5Widgets REQUIRED)

file( GLOB_RECURSE sources src/*.cpp )
file( GLOB_RECURSE headers src/*.hpp )
file( GLOB_RECURSE inlines src/*.inl )

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_definitions( ${Qt5Core_DEFINITIONS} )
add_definitions( ${Qt5Widges_DEFINITIONS} )
add_definitions( ${Qt5OpenGL_DEFINITIONS} )

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR} # MOC
    ${RADIUM_INCLUDE_DIR}
    ${EIGEN3_INCLUDE_DIR} )

add_library( Dummy SHARED ${sources} ${headers} ${inlines} )

add_dependencies( Dummy radiumCore radiumEngine radium )

target_compile_definitions( Dummy PRIVATE "-DDummy_EXPORTS" )

set(Qt5_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5OpenGL_LIBRARIES})
target_link_libraries( Dummy
                       ${Qt5_LIBRARIES}
                       ${RADIUM_LIBRARIES}
                     )


