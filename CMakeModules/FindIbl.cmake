
#
# Try to find Ibl library and include path.
# Once done this will define
#
# Ibl_FOUND
# IBL_INCLUDE_DIR
# IBL_LIBRARY
#

FIND_PATH( IBL_INCLUDE_DIR ibl/base/ImplicitBase.hpp
        ${CMAKE_SOURCE_DIR}/3rdPartyLibraries/ibl
        ${CMAKE_SOURCE_DIR}/../Radium-Engine-Plugins/ibl/include
        ${CMAKE_SOURCE_DIR}/../Radium-Engine-Plugins/ibl/
        DOC "The ibl include directory")

FIND_LIBRARY( IBL_LIBRARY
        NAMES ibl libibl.a ibl.lib
        PATHS
        ${CMAKE_SOURCE_DIR}/3rdPartyLibraries/ibl/Release/ibl
        ${CMAKE_SOURCE_DIR}/../Radium-Engine-Plugins/ibl/
        ${CMAKE_SOURCE_DIR}/../Radium-Engine-Plugins/ibl/Release/
        ${CMAKE_SOURCE_DIR}/../Radium-Engine-Plugins/ibl/Debug/
        DOC "The Ibl library")


IF (IBL_INCLUDE_DIR AND IBL_LIBRARY)
    SET( Ibl_FOUND TRUE)
ENDIF (IBL_INCLUDE_DIR AND IBL_LIBRARY)

SET(Ibl_FIND_QUIETLY FALSE)
IF(Ibl_FOUND)
   IF(NOT Ibl_FIND_QUIETLY)
      MESSAGE(STATUS "Found Ibl: ${IBL_LIBRARY}")
   ENDIF(NOT Ibl_FIND_QUIETLY)
ELSE(Ibl_FOUND)
   IF(Ibl_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find Ibl")
   ENDIF(Ibl_FIND_REQUIRED)
ENDIF(Ibl_FOUND)
