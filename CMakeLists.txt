project(RadiumEngine)

# Minimal version 3.8 required to get C++17 support
# https://cmake.org/cmake/help/v3.8/prop_tgt/CXX_STANDARD.html
cmake_minimum_required(VERSION 3.8)

if (APPLE)
# CMake 2.8.12 and newer has support for using @rpath in a target’s install
# name. This was enabled by setting the target property MACOSX_RPATH. The
# @rpath in an install name is a more flexible and powerful mechanism than
# @executable_path or @loader_path for locating shared libraries.

# CMake 3.0 and later prefer this property to be ON by default. Projects wanting
# @rpath in a target’s install name may remove any setting of the
# INSTALL_NAME_DIR and CMAKE_INSTALL_NAME_DIR variables.

# This policy was introduced in CMake version 3.0. CMake version 3.0.2 warns
# when the policy is not set and uses OLD behavior. Use the cmake_policy command
# to set it to OLD or NEW explicitly.
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
option(RADIUM_ASSIMP_SUPPORT        "Enable assimp loader" ON)
option(RADIUM_TINYPLY_SUPPORT       "Enable TinyPly loader" ON)
option(RADIUM_BUILD_APPS            "Choose to build or not radium applications" ON)
option(RADIUM_FAST_MATH             "Enable Fast Math optimizations in Release Mode (ignored with MVSC)" OFF)

if ( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Debug )
endif()


# These paths need to be synchronized with FindRadium.cmake
set(RADIUM_BUNDLE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bundle-${CMAKE_CXX_COMPILER_ID})
set(RADIUM_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(RADIUM_PLUGINS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Plugins")
set(RADIUM_PLUGIN_OUTPUT_PATH "${RADIUM_BUNDLE_DIRECTORY}/${CMAKE_BUILD_TYPE}/bin/Plugins")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${RADIUM_BUNDLE_DIRECTORY}/${CMAKE_BUILD_TYPE}/bin)
set(EXECUTABLE_OUTPUT_PATH         ${RADIUM_BUNDLE_DIRECTORY}/${CMAKE_BUILD_TYPE}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${RADIUM_BUNDLE_DIRECTORY}/${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${RADIUM_BUNDLE_DIRECTORY}/${CMAKE_BUILD_TYPE}/lib)

# get changeset id
find_package(Git)

if(GIT_FOUND)
  execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_CHANGESET)
    # remove new line sometime appearing in git changeset
    string(REGEX REPLACE "\n$" "" GIT_CHANGESET "${GIT_CHANGESET}")
else()
  set(GIT_CHANGESET "")
endif()
message("Git Changeset: ${GIT_CHANGESET}")


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
        set(PDB_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    else()
        foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
            string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
            set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
            set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
            set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
        endforeach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    endif()
endif(MSVC OR MSVC_IDE)


# ----------------------------------------------------------------------------------------------------------------------
#   SUBMODULES : assimp, glbinding, ....
include(SubModules)
set( RADIUM_INCLUDE_DIRS)
list(APPEND RADIUM_INCLUDE_DIRS "${RADIUM_SRC_DIR}" "${EIGEN3_INCLUDE_DIR}" "${ASSIMP_INCLUDE_DIR}" "${GLBINDING_INCLUDE_DIR}" "${GLOBJECTS_INCLUDE_DIR}" "${OPENMESH_INCLUDE_DIR}")

# ----------------------------------------------------------------------------------------------------------------------
#   Main Libraries
# Configure RPath once submodules are configured
include(ConfigureRPath)

set(RADIUM_LIBRARIES)
# Radium libraries are set in RADIUM_SRC_DIR
add_subdirectory(${RADIUM_SRC_DIR})
#message(INFO " Radium Lib (general) ${RADIUM_LIBRARIES}")

# ----------------------------------------------------------------------------------------------------------------------
#   Applications
#   Set Base directories for dependencies
if(RADIUM_BUILD_APPS)
  add_subdirectory(Applications)
endif(RADIUM_BUILD_APPS)

# ----------------------------------------------------------------------------------------------------------------------
#   External plugins
add_subdirectory(${RADIUM_PLUGINS_DIR})


################################################################################
# add a target to generate API documentation with Doxygen                      #
################################################################################

find_package(Doxygen)
if(DOXYGEN_FOUND)

  set (RADIUM_PROJECT_NUMBER ${GIT_CHANGESET})
  set (RADIUM_DOC_DIRECTORY ${RADIUM_BUNDLE_DIRECTORY}/Docs)

  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Docs/Doxyfile.in ${RADIUM_BUNDLE_DIRECTORY}/Doxyfile @ONLY)

  add_custom_target(doc
     ${CMAKE_COMMAND} -E make_directory ${RADIUM_DOC_DIRECTORY}
     COMMAND ${CMAKE_COMMAND} -E copy_directory
       ${CMAKE_CURRENT_SOURCE_DIR}/Docs/images/
       ${RADIUM_DOC_DIRECTORY}/html/images
     COMMAND ${CMAKE_COMMAND} -E copy
       ${CMAKE_CURRENT_SOURCE_DIR}/CHANGELOG
       ${RADIUM_BUNDLE_DIRECTORY}/
     COMMAND ${DOXYGEN_EXECUTABLE} ${RADIUM_BUNDLE_DIRECTORY}/Doxyfile
     DEPENDS
       ${CMAKE_CURRENT_SOURCE_DIR}/Docs/Doxyfile.in
       ${CMAKE_CURRENT_SOURCE_DIR}/CHANGELOG
     WORKING_DIRECTORY ${RADIUM_BUNDLE_DIRECTORY}
     COMMENT "Generating API documentation with Doxygen" VERBATIM
  )
endif(DOXYGEN_FOUND)

################################################################################
# Assets                                                                       #
################################################################################
#
set(SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Shaders")

if(MSVC OR MSVC_IDE OR MINGW)
add_custom_target( radium_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SHADER_DIR} "${EXECUTABLE_OUTPUT_PATH}/Shaders"
    COMMENT "Copying shaders and other ressources" VERBATIM
)
else()
    add_custom_target( radium_assets
            COMMAND ${CMAKE_COMMAND} -E create_symlink ${SHADER_DIR} "${EXECUTABLE_OUTPUT_PATH}/Shaders"
            COMMENT "Linking (symlink) shaders and other ressources" VERBATIM
            )
endif()
add_dependencies(radium_assets radiumCore)

################################################################################
# Configs                                                                      #
################################################################################
set(CONFIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Configs")

if(MSVC OR MSVC_IDE OR MINGW)
    add_custom_target( radium_configs
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CONFIG_DIR} "${EXECUTABLE_OUTPUT_PATH}/Configs"
        COMMENT "Copying configs (keymapping)" VERBATIM
        )
else()
    add_custom_target( radium_configs
            COMMAND ${CMAKE_COMMAND} -E create_symlink ${CONFIG_DIR} "${EXECUTABLE_OUTPUT_PATH}/Configs"
            COMMENT "Linking (symlink) configs (keymapping)" VERBATIM
            )
endif()
add_dependencies(radium_configs radiumCore)
