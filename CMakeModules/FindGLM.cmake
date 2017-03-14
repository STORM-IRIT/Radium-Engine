
# GLM_FOUND
# GLM_INCLUDE_DIR

include(FindPackageHandleStandardArgs)
set(glm_DIR ${CMAKE_SOURCE_DIR}/3rdPartyLibraries/OpenGL/glm)

FIND_PATH(GLM_INCLUDE_DIR glm/glm.hpp

    PATHS
    $ENV{GLM_DIR}
	${glm_DIR}
	/usr
	/usr/local
    /sw
    /opt/local

	PATH_SUFFIXES
    /include

    DOC "The directory where glm/glm.hpp resides.")
    
find_package_handle_standard_args(GLM REQUIRED_VARS GLM_INCLUDE_DIR)

mark_as_advanced(GLM_INCLUDE_DIR)

SET(GLM_FIND_QUIETLY FALSE)
IF(GLM_FOUND)
	IF(NOT GLM_FIND_QUIETLY)
		MESSAGE(STATUS "Found glm: ${GLM_INCLUDE_DIR}")
	ENDIF(NOT GLM_FIND_QUIETLY)
ELSE(GLM_FOUND)
	IF(GLM_FIND_REQUIRED)
		MESSAGE(FATAL_ERROR "Could not find glm headers")
	ENDIF(GLM_FIND_REQUIRED)
ENDIF(GLM_FOUND)
