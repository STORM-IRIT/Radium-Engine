project(RadiumEngine)

cmake_minimum_required(VERSION 2.8.11)

if ((${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang"))
    # No openmp on MacosX Clang (TODO, find better compiler identification)
    set(CMAKE_CXX_FLAGS                "-Wall -Wextra  -std=c++1y -msse3 -Wno-sign-compare -Wno-unused-parameter -fno-exceptions ${CMAKE_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG          "-D_DEBUG -DCORE_DEBUG -g3 -ggdb ${CMAKE_CXX_FLAGS_DEBUG}")
    set(CMAKE_CXX_FLAGS_RELEASE        "-DNDEBUG -O3 -ffast-math -mfpmath=sse")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g3 ${CMAKE_CXX_FLAGS_RELEASE}")

    add_definitions( -Wno-deprecated-declarations ) # Do not warn for eigen bind being deprecated
elseif (UNIX)
  set(CMAKE_CXX_FLAGS                "-Wall -Wextra  -pthread -std=c++1y -msse3 -Wno-sign-compare -Wno-unused-parameter -fno-exceptions -fopenmp ${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS_DEBUG          "-D_DEBUG -DCORE_DEBUG -g3 -ggdb ${CMAKE_CXX_FLAGS_DEBUG}")
  set(CMAKE_CXX_FLAGS_RELEASE        "-DNDEBUG -O3 -ffast-math -mfpmath=sse")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g3 ${CMAKE_CXX_FLAGS_RELEASE}")

  add_definitions( -Wno-deprecated-declarations ) # Do not warn for eigen bind being deprecated
elseif (MSVC)
    # Visual studio flags breakdown
    # /GR- : no rtti ; /Ehs-c- : no exceptions
    # /Od  : disable optimization
    # /Ox :  maximum optimization
    # /GL : enable link time optimization
    # /Zi  : generate debug info
    #remove exceptions from default args
    add_definitions(-D_HAS_EXCEPTIONS=0)
    string (REGEX REPLACE "/EHsc *" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string (REGEX REPLACE "/GR" ""     CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

    set(CMAKE_CXX_FLAGS	               "/arch:AVX2 /GR- /EHs-c- /MP ${CMAKE_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG		   "/D_DEBUG /DCORE_DEBUG /Od /Zi ${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE		   "/DNDEBUG /Ox /fp:fast /GL /MT")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/Zi ${CMAKE_CXX_FLAGS_RELEASE}")
endif()

# Problem with Qt linking
# FIXME(Charly): Not sure if this should be done on Linux
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_COMPILING_QSTRING_COMPAT_CPP")

set( USE_DOUBLE False CACHE BOOL "Use double precision" )
if ("${USE_DOUBLE}" STREQUAL "True")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCORE_USE_DOUBLE")
  message("Compile using double precision.")
else()
  message("Compile using single precision.")
endif()

# CMake setups
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)

# Debug by default !
if ( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Debug )
endif()

set( VALID_CMAKE_BUILD_TYPES "Debug Release RelWithDebInfo" )
if ( NOT "${VALID_CMAKE_BUILD_TYPES}" MATCHES ${CMAKE_BUILD_TYPE} )
    set( CMAKE_BUILD_TYPE Debug )
endif()

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

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

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
