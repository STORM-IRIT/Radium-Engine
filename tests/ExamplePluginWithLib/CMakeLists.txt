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

project(ExamplePluginWithLib VERSION 1.0.0)

# Prevent the plugins to be installed in the Radium bundle. Install them locally.
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(PLUGIN_DEMO_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Plugins-${CMAKE_CXX_COMPILER_ID})
else ()
    set(PLUGIN_DEMO_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Plugins-${CMAKE_CXX_COMPILER_ID}-${CMAKE_BUILD_TYPE})
endif ()
message(STATUS " Installing plugin demo into ${PLUGIN_DEMO_DIRECTORY}")
# This make the build system to buuuuuuug when compiling before installing Radium
set(CMAKE_INSTALL_PREFIX ${PLUGIN_DEMO_DIRECTORY})
set(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT False)

add_subdirectory(Upstream)
#add_subdirectory(Downstream)

add_custom_target(${PROJECT_NAME}-Doc SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
