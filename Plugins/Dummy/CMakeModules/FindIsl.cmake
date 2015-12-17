
#
# Try to find GLEW library and include path.
# Once done this will define
#
# GLEW_FOUND
# GLEW_INCLUDE_DIR
# GLEW_LIBRARY
#

FIND_PATH( ISL_INCLUDE_DIR isl/ImplicitBuilder.hpp
        ${CMAKE_SOURCE_DIR}/3rdPartyLibraries/isl
        ${CMAKE_SOURCE_DIR}/../Radium-Engine-Plugins/isl/include
        DOC "The ibl include directory")

FIND_LIBRARY( ISL_LIBRARY
        NAMES isl libisl.a isl.lib
        PATHS
        ${CMAKE_SOURCE_DIR}/3rdPartyLibraries/isl/Release/isl
        ${CMAKE_SOURCE_DIR}/../Radium-Engine-Plugins/isl/Release/
        ${CMAKE_SOURCE_DIR}/../Radium-Engine-Plugins/isl/Debug/
        DOC "The Ibl library")


IF (ISL_INCLUDE_DIR AND ISL_LIBRARY)
    SET( ISL_FOUND TRUE)
ENDIF (ISL_INCLUDE_DIR AND ISL_LIBRARY)

SET(ISL_FIND_QUIETLY FALSE)
IF(ISL_FOUND)
   IF(NOT ISL_FIND_QUIETLY)
      MESSAGE(STATUS "Found ISL: ${ISL_LIBRARY}")
   ENDIF(NOT ISL_FIND_QUIETLY)
ELSE(ISL_FOUND)
   IF(ISL_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find ISL")
   ENDIF(ISL_FIND_REQUIRED)
ENDIF(ISL_FOUND)
