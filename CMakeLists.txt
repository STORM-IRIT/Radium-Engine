project(RadiumEngine)

cmake_minimum_required(VERSION 2.8.11)

# CMake setups
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Build options options.
option(RADIUM_WITH_DOUBLE_PRECISION "Use double precision" OFF)
option(RADIUM_WITH_OMP              "Use OpenMP" OFF)
option(RADIUM_WITH_FANCY_GL         "Enable Fancy OpenGL effects" ON)
option(RADIUM_WITH_TEXTURES         "Compile Radium enabling texture loading stuff" ON)
option(RADIUM_WITH_PROFILING        "Enable functions profiling." OFF)
option(RADIUM_WARNINGS_AS_ERRORS    "Treat compiler warning as errors" OFF)

if ( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Debug )
endif()


# get changeset id
find_package(Git)
if(GIT_FOUND)
  execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_CHANGESET)
else()
  set(GIT_CHANGESET "")
endif()

# guard against in-source builds (source: Eigen)
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt. ")
endif()

# Set the external project assimp
include(ExternalAssimp)

# Set the compiler flags.
include(CompileFlags)

# Problem with Qt linking
# FIXME(Charly): Not sure if this should be done on Linux
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_COMPILING_QSTRING_COMPAT_CPP")

# These paths need to be synchronized with FindRadium.cmake
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_BUILD_TYPE}/bin)
set(EXECUTABLE_OUTPUT_PATH         ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_BUILD_TYPE}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_BUILD_TYPE}/lib)

if ( NOT CMAKE_PREFIX_PATH )
  set( CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR} )
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
        set(PDB_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_BUILD_TYPE}/bin)
    else()
        foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
            string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
            set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_BUILD_TYPE}/bin)
            set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_BUILD_TYPE}/lib)
            set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_BUILD_TYPE}/lib)
        endforeach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    endif()
endif(MSVC OR MSVC_IDE)

# EXTERNALS
# TODO Move this in another cmake config file
set(EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdPartyLibraries)
add_subdirectory(3rdPartyLibraries)

# Eigen
set( EIGEN_INC ${EXTERNAL_DIR}/Eigen )

# Assimp
set( ASSIMP_INC ${EXTERNAL_DIR}/Assimp/include )
if( APPLE )
    if ( ${CMAKE_BUILD_TYPE} STREQUAL "Debug" )
        set( ASSIMP_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/libassimpd.dylib" )
    else()
        set( ASSIMP_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/libassimp.dylib" )
    endif()
elseif ( UNIX )
    if ( ${CMAKE_BUILD_TYPE} STREQUAL "Debug" )
        set( ASSIMP_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/libassimpd.so" )
    else()
        set( ASSIMP_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/libassimp.so" )
    endif()
elseif (MINGW)
    if ( ${CMAKE_BUILD_TYPE} STREQUAL "Debug" )
        set( ASSIMP_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/libassimpd.dll.a" )
    else()
        set( ASSIMP_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/libassimp.dll.a" )
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

    set(ASSIMP_LIB optimized "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp-${MSVC_PREFIX}-mt.lib"
                 debug "${CMAKE_CURRENT_SOURCE_DIR}/lib/assimp-${MSVC_PREFIX}-mtd.lib")

endif()

set( CMAKE_DEBUG_POSTFIX "" )
set( OMIT_MAIN_APP False CACHE BOOL "Choose to build or not the main radium application" )

add_subdirectory(src)
add_subdirectory(Applications)

# FIXME(Charly) Not sure this is really the greatest idea ever
add_subdirectory(Plugins)


################################################################################
# add a target to generate API documentation with Doxygen                      #
################################################################################

find_package(Doxygen)
if(DOXYGEN_FOUND)
  set (RADIUM_PROJECT_NUMBER ${GIT_CHANGESET})

  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Docs/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

  add_custom_target(doc
     ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/Docs
     COMMAND ${CMAKE_COMMAND} -E copy_directory
       ${CMAKE_CURRENT_SOURCE_DIR}/Docs/images/
       ${CMAKE_CURRENT_BINARY_DIR}/Docs/html/images
     COMMAND ${CMAKE_COMMAND} -E copy
       ${CMAKE_CURRENT_SOURCE_DIR}/CHANGELOG
       ${CMAKE_CURRENT_BINARY_DIR}/Docs/
     COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
     DEPENDS
       ${CMAKE_CURRENT_SOURCE_DIR}/Docs/Doxyfile.in
       ${CMAKE_CURRENT_SOURCE_DIR}/CHANGELOG
     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
     COMMENT "Generating API documentation with Doxygen" VERBATIM
  )
endif(DOXYGEN_FOUND)

################################################################################
# Assets                                                                       #
################################################################################
#
set(SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Shaders")

add_custom_target( radium_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SHADER_DIR} "${EXECUTABLE_OUTPUT_PATH}/Shaders"
    COMMENT "Copying shaders and other ressources" VERBATIM
)
