
#
# Try to find GLEW library and include path.
# Once done this will define
#
# GLEW_FOUND
# GLEW_INCLUDE_DIR
# GLEW_LIBRARY
#

if (MSVC)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set( WIN_FOLDER_NAME "x64")
    else()
        set( WIN_FOLDER_NAME "Win32")
    endif()
elseif (MINGW)
    set (WIN_FOLDER_NAME "mingw")
endif()

FIND_PATH( GLEW_INCLUDE_DIR GL/glew.h
		${CMAKE_SOURCE_DIR}/3rdPartyLibraries/Glew/include
		/usr/include
		/usr/local/include
		/sw/include
		/opt/local/include
		DOC "The directory where GL/glew.h resides")

FIND_LIBRARY( GLEW_LIBRARY
                NAMES GLEW glew glew32 libglew32 libglew32.a glew32.lib
		PATHS
                ${CMAKE_SOURCE_DIR}/3rdPartyLibraries/Glew/lib/Release/${WIN_FOLDER_NAME}
		/usr/lib64
		/usr/lib
		/usr/local/lib64
		/usr/local/lib
		/sw/lib
		/opt/local/lib
		DOC "The GLEW library")

IF (GLEW_INCLUDE_DIR AND GLEW_LIBRARY)
	SET( GLEW_FOUND TRUE)
	SET( GLEW_LIBRARIES ${GLEW_LIBRARY})
ENDIF (GLEW_INCLUDE_DIR AND GLEW_LIBRARY)

SET(GLEW_FIND_QUIETLY FALSE)
IF(GLEW_FOUND)
   IF(NOT GLEW_FIND_QUIETLY)
      MESSAGE(STATUS "Found GLEW: ${GLEW_LIBRARY}")
   ENDIF(NOT GLEW_FIND_QUIETLY)
ELSE(GLEW_FOUND)
   IF(GLEW_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find GLEW")
   ENDIF(GLEW_FIND_REQUIRED)
ENDIF(GLEW_FOUND)
