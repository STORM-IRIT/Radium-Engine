project(RadiumEngine)

cmake_minimum_required(VERSION 2.8.11)

if (APPLE)
cmake_policy(SET CMP0042 NEW)
endif(APPLE)

# CMake setups
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Build options options.
option(RADIUM_WITH_DOUBLE_PRECISION "Use double precision" OFF)
option(RADIUM_WITH_OMP              "Use OpenMP" OFF)
option(RADIUM_WITH_FANCY_GL         "Enable Fancy OpenGL effects" ON)
option(RADIUM_WITH_TEXTURES         "Compile Radium enabling texture loading stuff" ON)
option(RADIUM_WITH_PROFILING        "Enable functions profiling." OFF)
option(RADIUM_WARNINGS_AS_ERRORS    "Treat compiler warning as errors" OFF)
option(RADIUM_FORCE_ASSERTS         "Enable asserts regardless of build type" OFF)

if ( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Debug )
endif()


# These paths need to be synchronized with FindRadium.cmake
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_BUILD_TYPE}/bin)
set(EXECUTABLE_OUTPUT_PATH         ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_BUILD_TYPE}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_BUILD_TYPE}/lib)
set(RADIUM_SUBMODULES_INSTALL_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_BUILD_TYPE}/3rdPartyLibraries)


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


# Set the compiler flags.
include(CompileFlags)


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


# ----------------------------------------------------------------------------------------------------------------------
#   SUBMODULES : assimp, glbinding, ....
include(SubModules)

# ----------------------------------------------------------------------------------------------------------------------
#   Main Libraries
set( CMAKE_DEBUG_POSTFIX "" )
set( OMIT_MAIN_APP False CACHE BOOL "Choose to build or not the main radium application" )

add_subdirectory(src)

# ----------------------------------------------------------------------------------------------------------------------
#   Applications
#   Set Base directories for dependencies
# use Eigen version included in the Radium distribution, not the system-wide installed if available

#set (EIGEN3_DIR ${RADIUM_SUBMODULES_INSTALL_DIRECTORY})
#set (GLBINDING_DIR ${RADIUM_SUBMODULES_INSTALL_DIRECTORY})

add_subdirectory(Applications)


# ----------------------------------------------------------------------------------------------------------------------
#   External plugins
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

################################################################################
# Configs                                                                      #
################################################################################
set(CONFIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Configs")

add_custom_target( radium_configs
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CONFIG_DIR} "${EXECUTABLE_OUTPUT_PATH}/Configs"
        COMMENT "Copying configs (keymapping)" VERBATIM
        )