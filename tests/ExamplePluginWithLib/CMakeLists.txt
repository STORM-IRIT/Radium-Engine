cmake_minimum_required(VERSION 3.6)
# ------------------------------------------------------------------------------
# Policies and global parameters for CMake
if(POLICY CMP0077)
    # allow to define options cache variable before the option is declared
    # https://cmake.org/cmake/help/latest/policy/CMP0077.html
    cmake_policy(SET CMP0077 NEW)
endif()
if(APPLE)
    # MACOSX_RPATH is enabled by default. https://cmake.org/cmake/help/latest/policy/CMP0042.html
    cmake_policy(SET CMP0042 NEW)
endif(APPLE)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    message("Set default build type to ${CMAKE_BUILD_TYPE}")
endif()

project(ExamplePluginWithLib VERSION 1.0.0)

# Prevent the plugins to be installed in the Radium bundle. Install them locally. TODO : modify this
# so that, if this example is configured and compiled alone (outside of Radium build tree) the user
# might select its install dir
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(PLUGIN_DEMO_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Plugins-${CMAKE_CXX_COMPILER_ID})
else()
    set(PLUGIN_DEMO_DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR}/Plugins-${CMAKE_CXX_COMPILER_ID}-${CMAKE_BUILD_TYPE}
    )
endif()
message(STATUS "Installing plugin demo into ${PLUGIN_DEMO_DIRECTORY}")

# install the demo in ${PLUGIN_DEMO_DIRECTORY}
set(CMAKE_INSTALL_PREFIX ${PLUGIN_DEMO_DIRECTORY})
set(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT False)

# configure the demo
add_custom_target(${PROJECT_NAME})

# Upstream library and plugin
add_subdirectory(Upstream)
add_dependencies(ExamplePluginWithLib UpstreamLibrary UpstreamPlugin)

# Downstream plugin that search for the package UpstreamLibrary allow to find_package in the build
# tree
set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${CMAKE_CURRENT_BINARY_DIR}/Upstream/Library"})
add_subdirectory(Downstream)
add_dependencies(ExamplePluginWithLib DownstreamPlugin)

# documentation (to be updated)
add_custom_target(${PROJECT_NAME}-Doc SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
