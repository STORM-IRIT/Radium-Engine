cmake_minimum_required(VERSION 3.18)
# ------------------------------------------------------------------------------
# Policies and global parameters for CMake
if(POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)
endif()
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    message("Set default build type to ${CMAKE_BUILD_TYPE}")
endif()

project(DownstreamPlugin VERSION 1.0.0)

if(APPLE)
    cmake_policy(SET CMP0042 NEW)
endif(APPLE)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/installed-${CMAKE_CXX_COMPILER_ID}"
        CACHE PATH "Install path prefix, prepended onto install directories." FORCE
    )
    message("Set install prefix to ${CMAKE_INSTALL_PREFIX}")
    set(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT False)
endif()

add_subdirectory(Plugin)

add_custom_target(${PROJECT_NAME}-Doc SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
