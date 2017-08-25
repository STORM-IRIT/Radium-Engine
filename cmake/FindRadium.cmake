# Try to find the radium engine base folder
# Will define
# RADIUM_ROOT_DIR : the root of the radium SDK
# RADIUM_LIBRARIES
# RADIUM_INCLUDE_DIRS : the include directories of radium
# RADIUM_PLUGIN_OUTPUT_PATH : output path for radiums plugin
# RADIUM_BINARY_OUTPUT_PATH : output path for radiums external binaries
# RADIUM_SUBMODULES_BUILD_TYPE : build type of the 3rdparties
# RADIUM_SUBMODULES_INSTALL_DIRECTORY : install directory for 3rdparties
# ASSIMP_LIBRARIES, GLBINDING_LIBRARIES, GLOBJECTS_LIBRARIES


# Radium_FOUND if found
if( NOT RADIUM_ROOT_DIR )
  find_path( RADIUM_ROOT_DIR NAMES src/Core/RaCore.hpp
    PATHS
    "${CMAKE_SOURCE_DIR}/extern"
    "${CMAKE_SOURCE_DIR}/external"
    "${CMAKE_SOURCE_DIR}/3rdPartyLibraries"
    "${CMAKE_SOURCE_DIR}/.."
    "${CMAKE_SOURCE_DIR}/../.."
    "${CMAKE_SOURCE_DIR}/../../.."
    "${CMAKE_CURRENT_SOURCE_DIR}/extern"
    "${CMAKE_CURRENT_SOURCE_DIR}/external"
    "${CMAKE_CURRENT_SOURCE_DIR}/.."
    "${CMAKE_CURRENT_SOURCE_DIR}/../.."
    "${CMAKE_CURRENT_SOURCE_DIR}/../../.."
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdPartyLibraries"
    PATH_SUFFIXES Radium-Engine
    DOC "The radium engine source folder")
endif( NOT RADIUM_ROOT_DIR )

if ( RADIUM_ROOT_DIR )
  set ( RADIUM_SRC_DIR "${RADIUM_ROOT_DIR}/src")
  set ( RADIUM_BUNDLE_DIRECTORY "${RADIUM_ROOT_DIR}/Bundle-${CMAKE_CXX_COMPILER_ID}")
  set ( RADIUM_BINARY_OUTPUT_PATH "${RADIUM_BUNDLE_DIRECTORY}/${CMAKE_BUILD_TYPE}/bin")
  set ( RADIUM_PLUGIN_OUTPUT_PATH "${RADIUM_BUNDLE_DIRECTORY}/${CMAKE_BUILD_TYPE}/bin/Plugins")

  if(MSVC OR MSVC_IDE)
    set(RADIUM_SUBMODULES_INSTALL_DIRECTORY "${RADIUM_BUNDLE_DIRECTORY}/${CMAKE_BUILD_TYPE}/3rdPartyLibraries" )
    set(RADIUM_SUBMODULES_BUILD_TYPE "${CMAKE_BUILD_TYPE}")
  else()
    set(RADIUM_SUBMODULES_INSTALL_DIRECTORY "${RADIUM_BUNDLE_DIRECTORY}/3rdPartyLibraries" )
    set(RADIUM_SUBMODULES_BUILD_TYPE Release)
  endif()

  find_library( RA_CORE_LIB
    NAMES radiumCore
    PATHS "${RADIUM_BUNDLE_DIRECTORY}/${CMAKE_BUILD_TYPE}/lib"
    )
  find_library( RA_ENGINE_LIB
    NAMES radiumEngine
    PATHS "${RADIUM_BUNDLE_DIRECTORY}/${CMAKE_BUILD_TYPE}/lib"
    )
  find_library( RA_IO_LIB
    NAMES radiumIO
    PATHS "${RADIUM_BUNDLE_DIRECTORY}/${CMAKE_BUILD_TYPE}/lib"
    )
  find_library( RA_GUIBASE_LIB
    NAMES radiumGuiBase
    PATHS "${RADIUM_BUNDLE_DIRECTORY}/${CMAKE_BUILD_TYPE}/lib"
    )

  set ( Radium_FOUND TRUE )
  ############################################################################
  # Get dependencies if not already specified
  if(NOT EIGEN3_INCLUDE_DIR)
    set(EIGEN3_INCLUDE_DIR "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/include" )
  endif(NOT EIGEN3_INCLUDE_DIR)

  if (NOT ASSIMP_LIBRARIES)
    find_library ( ASSIMP_LIBRARIES
      NAMES assimp assimpd
      PATHS "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib"
      )
  endif (NOT ASSIMP_LIBRARIES)

  if(NOT ASSIMP_INCLUDE_DIR)
    set(ASSIMP_INCLUDE_DIR "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/include" )
  endif(NOT ASSIMP_INCLUDE_DIR)

  if(NOT GLM_INCLUDE_DIR)
    set(GLM_INCLUDE_DIR "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/include" )
  endif(NOT GLM_INCLUDE_DIR)

  if (NOT GLBINDING_LIBRARIES)
    find_library ( GLBINDING_LIBRARIES
      NAMES glbinding glbindingd
      PATHS "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib"
      )
  endif (NOT GLBINDING_LIBRARIES)

  if(NOT GLBINDING_INCLUDE_DIR)
    set(GLBINDING_INCLUDE_DIR "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/include" )
  endif(NOT GLBINDING_INCLUDE_DIR)

  if (NOT GLOBJECTS_LIBRARIES)
    find_library ( GLOBJECTS_LIBRARIES
      NAMES globjects globjectsd
      PATHS "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib"
      )
  endif (NOT GLOBJECTS_LIBRARIES)

  if(NOT GLOBJECTS_INCLUDE_DIR)
    set(GLOBJECTS_INCLUDE_DIR "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/include" )
  endif(NOT GLOBJECTS_INCLUDE_DIR)

  set( RADIUM_INCLUDE_DIRS)
  list(APPEND RADIUM_INCLUDE_DIRS "${RADIUM_SRC_DIR}" "${EIGEN3_INCLUDE_DIR}" "${ASSIMP_INCLUDE_DIR}" "${GLBINDING_INCLUDE_DIR}" "${GLOBJECTS_INCLUDE_DIR}")

  set( RADIUM_LIBRARIES )
  if ( RA_CORE_LIB AND RA_ENGINE_LIB AND RA_IO_LIB AND RA_GUIBASE_LIB AND ASSIMP_LIBRARIES AND GLBINDING_LIBRARIES AND GLOBJECTS_LIBRARIES)
    list(APPEND RADIUM_LIBRARIES "${RA_CORE_LIB}" "${RA_ENGINE_LIB}" "${RA_IO_LIB}" "${RA_GUIBASE_LIB}")
    set ( Radium_Libs_FOUND TRUE)
  endif ( RA_CORE_LIB AND RA_ENGINE_LIB AND RA_IO_LIB AND RA_GUIBASE_LIB AND ASSIMP_LIBRARIES AND GLBINDING_LIBRARIES AND GLOBJECTS_LIBRARIES)
endif( RADIUM_ROOT_DIR )

if ( Radium_FOUND )
  if( NOT Radium_FIND_QUIETLY )
    message ( STATUS "Found Radium Engine: ${RADIUM_ROOT_DIR}")
    message ( STATUS "      Eigen3 includes: ${EIGEN3_INCLUDE_DIR}")
    message ( STATUS "      Glm includes: ${GLM_INCLUDE_DIR}")
    message ( STATUS "      Radium libs: ${RADIUM_LIBRARIES}")
    message ( STATUS "      Assimp libs: ${ASSIMP_LIBRARIES}")
    message ( STATUS "      GlBinding libs: ${GLBINDING_LIBRARIES}")
    message ( STATUS "      GlObjects libs: ${GLOBJECTS_LIBRARIES}")
  endif( NOT Radium_FIND_QUIETLY )
  if( NOT Radium_Libs_FOUND )
    message( WARNING "Could not find Radium libraries. You must compile them first")
  endif (NOT Radium_Libs_FOUND )
else( Radium_FOUND )
  if( Radium_FIND_REQUIRED )
    message( FATAL_ERROR "Could not find Radium Engine root dir")
  endif( Radium_FIND_REQUIRED )
endif( Radium_FOUND )
