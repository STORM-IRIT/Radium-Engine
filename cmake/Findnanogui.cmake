#
# Try to find the FreeImage library and include path.
# Once done this will define
#
# NANOGUI_FOUND
# NANOGUI_INCLUDE_DIR
# NANOGUI_LIBRARY
# 

FIND_PATH( NANOGUI_INCLUDE_DIR nanogui/nanogui.h
    ${PROJECT_SOURCE_DIR}/3rdPartyLibraries/nanogui/include
    ${PROJECT_SOURCE_DIR}/../nanogui/include
    /usr/include
    /usr/local/include
    /sw/include
    /opt/local/include
    DOC "The directory where nanogui.h resides")
FIND_LIBRARY( NANOGUI_LIBRARY
    NAMES nanogui  
    PATHS
    ${PROJECT_SOURCE_DIR}/3rdPartyLibraries/nanogui/build
    ${PROJECT_SOURCE_DIR}/../nanogui/build
    /usr/lib64
    /usr/lib
    /usr/local/lib64
    /usr/local/lib
    /sw/lib
    /opt/local/lib
    DOC "The FreeImage library")

SET(NANOGUI_LIBRARIES ${NANOGUI_LIBRARY})

IF (NANOGUI_INCLUDE_DIR AND NANOGUI_LIBRARY)
    SET( NANOGUI_FOUND TRUE CACHE BOOL "Set to TRUE if nanogui is found, FALSE otherwise")
ELSE (NANOGUI_INCLUDE_DIR AND NANOGUI_LIBRARY)
    SET( NANOGUI_FOUND FALSE CACHE BOOL "Set to TRUE if nanogui is found, FALSE otherwise")
ENDIF (NANOGUI_INCLUDE_DIR AND NANOGUI_LIBRARY)

MARK_AS_ADVANCED(
    NANOGUI_FOUND 
    NANOGUI_LIBRARY
    NANOGUI_LIBRARIES
    NANOGUI_INCLUDE_DIR)
