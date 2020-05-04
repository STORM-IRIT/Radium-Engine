cmake_minimum_required(VERSION 3.6)
#------------------------------------------------------------------------------
# Policies and global parameters for CMake
if (POLICY CMP0077)
    # allow to define options cache variable before the option is declared
    # https://cmake.org/cmake/help/latest/policy/CMP0077.html
    cmake_policy(SET CMP0077 NEW)
endif ()
if (APPLE)
    # MACOSX_RPATH is enabled by default.
    # https://cmake.org/cmake/help/latest/policy/CMP0042.html
    cmake_policy(SET CMP0042 NEW)
endif (APPLE)

# No in-source build
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    message("Set default build type to ${CMAKE_BUILD_TYPE}")
endif ()

project(PluginWithHelperLibDemo VERSION 1.0.0)

# Prevent the plugins to be installed in the Radium bundle. Install them locally.
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(PLUGIN_DEMO_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Plugins-${CMAKE_CXX_COMPILER_ID})
else ()
    set(PLUGIN_DEMO_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Plugins-${CMAKE_CXX_COMPILER_ID}-${CMAKE_BUILD_TYPE})
endif ()
message(STATUS " Installing plugin demo into ${PLUGIN_DEMO_DIRECTORY}")
# This make the build system to buuuuuuug when compiling before installing Radium
set(CMAKE_INSTALL_PREFIX ${PLUGIN_DEMO_DIRECTORY})

add_subdirectory(PluginA_WithHelperLib)
add_subdirectory(PluginB_UsingExternalHelperLib)
