project(RadiumEngine)

cmake_minimum_required(VERSION 2.8.11)

# CMake setups
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# Build options options.
set( USE_DOUBLE False CACHE BOOL "Use double precision" )
set( USE_OMP    False CACHE BOOL "Use OpenMP")

# Set the compiler flags.
include(CompileFlags)

# Problem with Qt linking
# FIXME(Charly): Not sure if this should be done on Linux
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_COMPILING_QSTRING_COMPAT_CPP")



set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH         ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

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

# EXTERNALS
# TODO Move this in another cmake config file
set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/3rdPartyLibraries)
add_subdirectory(3rdPartyLibraries)

# Eigen
set( EIGEN_INC ${EXTERNAL_DIR}/Eigen )

# Assimp
set( ASSIMP_INC ${EXTERNAL_DIR}/Assimp/include )
if( APPLE )
    if ( ${CMAKE_BUILD_TYPE} STREQUAL "Debug" )
        set( ASSIMP_LIB "${CMAKE_SOURCE_DIR}/lib/libassimpd.dylib" )
    else()
        set( ASSIMP_LIB "${CMAKE_SOURCE_DIR}/lib/libassimp.dylib" )
    endif()
elseif ( UNIX )
    if ( ${CMAKE_BUILD_TYPE} STREQUAL "Debug" )
        set( ASSIMP_LIB "${CMAKE_SOURCE_DIR}/lib/libassimpd.so" )
    else()
        set( ASSIMP_LIB "${CMAKE_SOURCE_DIR}/lib/libassimp.so" )
    endif()
elseif (MINGW)
    if ( ${CMAKE_BUILD_TYPE} STREQUAL "Debug" )
        set( ASSIMP_LIB "${CMAKE_SOURCE_DIR}/lib/libassimpd.dll.a" )
    else()
        set( ASSIMP_LIB "${CMAKE_SOURCE_DIR}/lib/libassimp.dll.a" )
    endif()
elseif( MSVC )
    # in order to prevent DLL hell, each of the DLLs have to be suffixed with the major version and msvc prefix
    if( MSVC70 OR MSVC71 )
        set(MSVC_PREFIX "vc70")
    elseif( MSVC80 )
        set(MSVC_PREFIX "vc80")
    elseif( MSVC90 )
        set(MSVC_PREFIX "vc90")
    elseif( MSVC10 )
        set(MSVC_PREFIX "vc100")
    elseif( MSVC11 )
        set(MSVC_PREFIX "vc110")
    elseif( MSVC12 )
        set(MSVC_PREFIX "vc120")
    else()
        set(MSVC_PREFIX "vc130")
    endif()

    set( ASSIMP_LIB "${CMAKE_SOURCE_DIR}/lib/assimp-${MSVC_PREFIX}-mt.lib" )
endif()

set( CMAKE_DEBUG_POSTFIX "" )
set( OMIT_MAIN_APP False CACHE BOOL "Choose to build or not the main radium application" )

set(LOAD_TEXTURES False CACHE BOOL "Compile Radium enabling texture loading stuff")
if (${LOAD_TEXTURES})
    message(STATUS "Textures will be loaded")
    add_definitions(-DLOAD_TEXTURES)
endif()

add_subdirectory(src)

# FIXME(Charly) Not sure this is really the greatest idea ever
add_subdirectory(Plugins)
