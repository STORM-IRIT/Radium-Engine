# - Try to find glbinding
# Once done, this will define
#
# GlBinding_FOUND - system has Assimp
# GlBinding_INCLUDE_DIR - the Assimp include directories
# GlBinding_LIBRARIES - link these to use Assimp

FIND_PATH( GlBinding_INCLUDE_DIR glbinding/glbinding_features.h
  ${SUBMODULES_OUTPUT_DIRECTORY}/glsubmodules/include
  /usr/include
  /usr/local/include
  /opt/local/include
)

FIND_LIBRARY( GlBinding_LIBRARY
  NAMES glbinding
  PATHS
  ${SUBMODULES_OUTPUT_DIRECTORY}/glsubmodules/lib/
  /usr/lib64
  /usr/lib
  /usr/local/lib
  /opt/local/lib
)

IF(GlBinding_INCLUDE_DIR AND GlBinding_LIBRARY)
  SET( GlBinding_FOUND TRUE )
  SET( GlBinding_LIBRARIES ${GlBinding_LIBRARY} )
ENDIF(GlBinding_INCLUDE_DIR AND GlBinding_LIBRARY)

SET(GlBinding_FIND_QUIETLY FALSE)
IF(GlBinding_FOUND)
   IF(NOT GlBinding_FIND_QUIETLY)
      MESSAGE(STATUS "Found glbinding: ${GlBinding_LIBRARY}")
   ENDIF(NOT GlBinding_FIND_QUIETLY)
ELSE(GlBinding_FOUND)
   IF(GlBinding_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find libglbinding")
   ENDIF(GlBinding_FIND_REQUIRED)
ENDIF(GlBinding_FOUND)
