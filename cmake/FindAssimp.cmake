# - Try to find Assimp
# Once done, this will define
#
# ASSIMP_FOUND - system has Assimp
# ASSIMP_INCLUDE_DIR - the Assimp include directories
# ASSIMP_LIBRARIES - link these to use Assimp

FIND_PATH( ASSIMP_INCLUDE_DIR NAMES assimp/mesh.h

  PATHS
  ${ASSIMP_DIR}
  /usr/include
  /usr/local/include
  /opt/local/include

  DOC "The directory where assimp/mesh.h resides."
  NO_DEFAULT_PATH
)

FIND_LIBRARY( ASSIMP_LIBRARY
  NAMES assimp assimp32.lib assimp.lib libassimp.a
  PATHS
  ${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib
  /usr/lib64
  /usr/lib
  /usr/local/lib
  /opt/local/lib
)

IF(ASSIMP_INCLUDE_DIR AND ASSIMP_LIBRARY)
  SET( ASSIMP_FOUND TRUE )
  SET( ASSIMP_LIBRARIES ${ASSIMP_LIBRARY} )
ENDIF(ASSIMP_INCLUDE_DIR ANDASSIMP_LIBRARY)

SET(ASSIMP_FIND_QUIETLY FALSE)
IF(ASSIMP_FOUND)
   IF(NOT ASSIMP_FIND_QUIETLY)
      MESSAGE(STATUS "Found Assimp: ${Assimp_LIBRARY}")
   ENDIF(NOT ASSIMP_FIND_QUIETLY)
ELSE(ASSIMP_FOUND)
   IF(ASSIMP_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find libAssimp")
   ENDIF(ASSIMP_FIND_REQUIRED)
ENDIF(ASSIMP_FOUND)
