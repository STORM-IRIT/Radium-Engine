project(RadiumEngine)

cmake_minimum_required(VERSION 2.8.11)

if (UNIX)
  set(CMAKE_CXX_FLAGS                "-Wall -Wextra  -pthread -std=c++11 -msse4.2 -Wno-sign-compare -Wno-unused-parameter -fno-rtti -fno-exceptions ${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS_DEBUG          "-D_DEBUG -DCORE_DEBUG -DOSG_NOTIFY_LEVEL=DEBUG -g3 -ggdb ${CMAKE_CXX_FLAGS_DEBUG}")
  set(CMAKE_CXX_FLAGS_RELEASE        "-DNDEBUG -O3 -ffast-math -mfpmath=sse")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g3 ${CMAKE_CXX_FLAGS_RELEASE}")
endif(UNIX)

# Visual studio flags breakdown
# /GR- : no rtti ; /Ehs-c- : no exceptions
# /Od  : disable optimization
# /Ox :  maximum optimization
# /GL : enable link time optimization
# /Zi  : generate debug info
if (MSVC)
  #remove exceptions from default args
  add_definitions(-D_HAS_EXCEPTIONS=0)
  string (REGEX REPLACE "/EHsc *" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  string (REGEX REPLACE "/GR" ""     CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

	set(CMAKE_CXX_FLAGS	               "/Wall /arch:SSE2 /arch:AVX2 /GR- /EHs-c- ${CMAKE_CXX_FLAGS}")
	set(CMAKE_CXX_FLAGS_DEBUG		   "/D_DEBUG /DCORE_DEBUG /DOSG_NOTIFY_LEVEL=DEBUG /Od /Zi ${CMAKE_CXX_FLAGS_DEBUG}")
	set(CMAKE_CXX_FLAGS_RELEASE		   "/DNDEBUG /Ox /fp:fast /GL")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/Zi ${CMAKE_CXX_FLAGS_RELEASE}")
endif(MSVC)

if ("${USE_DOUBLE}" STREQUAL "true")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCORE_USE_DOUBLE")
  message("Compile using double precision.")
else()
  message("Compile using single precision.")
endif()

# CMake setups
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)

# Debug by default !
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin-debug)
    set(EXECUTABLE_OUTPUT_PATH         ${CMAKE_SOURCE_DIR}/bin-debug)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib-debug)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib-debug)
    set(LIB_INSTALL_DIR                ${CMAKE_SOURCE_DIR}/lib-debug)
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
    set(EXECUTABLE_OUTPUT_PATH         ${CMAKE_SOURCE_DIR}/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
    set(LIB_INSTALL_DIR                ${CMAKE_SOURCE_DIR}/lib)
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
  foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/lib)
  endforeach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
endif(MSVC OR MSVC_IDE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# EXTERNALS
# TODO Move this in another cmake config file
set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/3rdPartyLibraries)

# Eigen
set(EIGEN_INC ${EXTERNAL_DIR}/Eigen)

add_subdirectory(3rdPartyLibraries)
add_subdirectory(src)



