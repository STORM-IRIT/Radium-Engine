project(FancyMeshPlugin)

cmake_minimum_required(VERSION 2.8.11)

#if ("${SSE_VERSION}" STREQUAL "3")

if (UNIX)
  set(CMAKE_CXX_FLAGS                "-Wall -Wextra  -pthread -std=c++11 -msse3 -Wno-sign-compare -Wno-unused-parameter -fno-rtti -fno-exceptions -fopenmp ${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS_DEBUG          "-D_DEBUG -DCORE_DEBUG -DOSG_NOTIFY_LEVEL=DEBUG -g3 -ggdb ${CMAKE_CXX_FLAGS_DEBUG}")
  set(CMAKE_CXX_FLAGS_RELEASE        "-DNDEBUG -O3 -ffast-math -mfpmath=sse")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g3 ${CMAKE_CXX_FLAGS_RELEASE}")

  add_definitions( -Wno-deprecated-declarations ) # Do not warn for eigen bind being deprecated
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

    set(CMAKE_CXX_FLAGS	               "/arch:AVX2 /GR- /EHs-c- /MP ${CMAKE_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG		   "/D_DEBUG /DCORE_DEBUG /DOSG_NOTIFY_LEVEL=DEBUG /Od /Zi ${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE		   "/DNDEBUG /Ox /fp:fast /GL /MT")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/Zi ${CMAKE_CXX_FLAGS_RELEASE}")
endif(MSVC)

# Problem with Qt linking
# FIXME(Charly): Not sure if this should be done on Linux
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_COMPILING_QSTRING_COMPAT_CPP")

if ("${USE_DOUBLE}" STREQUAL "true")
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

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# Eigen
set(EIGEN_INC ${EXTERNAL_DIR}/Eigen)

#find_package( RadiumEngine  REQUIRED )
find_package( OpenGL        REQUIRED )
find_package( GLEW          REQUIRED )
find_package( Assimp        REQUIRED )
find_package( Eigen3        REQUIRED )
find_package( Qt5Core       REQUIRED )
find_package( Qt5Widgets    REQUIRED )
find_package( Qt5OpenGL     REQUIRED )

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
    ${Assimp_INCLUDE_DIR}
    ${EIGEN3_INCLUDE_DIR} )

add_library( FancyMesh SHARED ${sources} ${headers} ${inlines} )

target_compile_definitions( FancyMesh PRIVATE "-DRA_CORE_IMPORTS" )
target_compile_definitions( FancyMesh PRIVATE "-DRA_ENGINE_IMPORTS" )
target_compile_definitions( FancyMesh PRIVATE "-DFancyMesh_EXPORTS" )

set(Qt5_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5OpenGL_LIBRARIES})
target_link_libraries( FancyMesh
                       ${Qt5_LIBRARIES}
                       ${RADIUM_LIBRARIES}
                       ${GLEW_LIBRARIES}
                       ${Assimp_LIBRARIES}
                       ${FREEIMAGE_LIBRARIES}
                       ${OPENGL_LIBRARIES} )


