# Locate the glfw3 library
#
# This module defines the following variables:
#
# GLFW_LIBRARY the name of the library;
# GLFW_INCLUDE_DIR where to find glfw include files.
# GLFW_FOUND true if both the GLFW3LIBRARY and GLFW_INCLUDE_DIR have been found.
#
# To help locate the library and include file, you can define a
# variable called GLFW3_ROOT which points to the root of the glfw library
# installation.
#
# default search dirs
# 
# Cmake file from the Learn openGL project
# https://github.com/JoeyDeVries/LearnOpenG
# Released under a public domain licence

set( _glfw3_HEADER_SEARCH_DIRS
    "/usr/include"
    "/usr/local/include"
    "${PROJECT_SOURCE_DIR}/3rdPartyLibraries/glfw3/include"
    "${CMAKE_SOURCE_DIR}/includes"
    "C:/Program Files (x86)/glfw/include" )

set( _glfw3_LIB_SEARCH_DIRS
    "/usr/lib"
    "/usr/lib/x86_64-linx-gnu"
    "/usr/local/lib"
    "${PROJECT_SOURCE_DIR}/3rdPartyLibraries/glfw3/build"
    "${CMAKE_SOURCE_DIR}/lib"
    "C:/Program Files (x86)/glfw/lib-msvc110" )

# Check environment for root search directory
set( _glfw3_ENV_ROOT $ENV{GLFW3_ROOT} )
if( NOT GLFW3_ROOT AND _glfw3_ENV_ROOT )
    set(GLFW3_ROOT ${_glfw3_ENV_ROOT} )
endif()

# Put user specified location at beginning of search
if( GLFW3_ROOT )
    list( INSERT _glfw3_HEADER_SEARCH_DIRS 0 "${GLFW3_ROOT}/include" )
    list( INSERT _glfw3_LIB_SEARCH_DIRS 0 "${GLFW3_ROOT}/lib" )
endif()

# Search for the header
FIND_PATH(GLFW_INCLUDE_DIR "GLFW/glfw3.h"
    PATHS ${_glfw3_HEADER_SEARCH_DIRS} )

# Search for the library
FIND_LIBRARY(GLFW_LIBRARY NAMES glfw3 glfw libglfw.so
    PATHS ${_glfw3_LIB_SEARCH_DIRS} )

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GLFW DEFAULT_MSG    GLFW_LIBRARY GLFW_INCLUDE_DIR)

