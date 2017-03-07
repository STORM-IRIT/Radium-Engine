# Try to find the radium engine base folder
# Will define
# RADIUM_ROOT : the root of the radium SDK
# RADIUM_INCLUDE_DIR : the include directory of radium
# EIGEN_INCLUDE_DIR : the eigen directory


# Radium_FOUND if found


FIND_PATH( RADIUM_ROOT NAMES src/Core/RaCore.hpp
    PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}/../Radium-Engine
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Radium-Engine
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../Radium-Engine
    DOC "The radium engine source folder")

IF ( RADIUM_ROOT )
    SET ( RADIUM_INCLUDE_DIR "${RADIUM_ROOT}/src")
    SET ( EIGEN_INCLUDE_DIR "${RADIUM_ROOT}/3rdPartyLibraries/Eigen")
    SET ( ASSIMP_INCLUDE_DIR "${RADIUM_ROOT}/3rdPartyLibraries/Assimp/include")

    IF (TARGET radiumCore)
        set (RA_CORE_LIB radiumCore)
    ELSE()
        FIND_LIBRARY( RA_CORE_LIB
            NAMES radiumCore
            PATHS ${RADIUM_ROOT}/${CMAKE_BUILD_TYPE}/lib
            )
    ENDIF()

    IF (TARGET radiumEngine)
        set (RA_ENGINE_LIB radiumEngine)
    ELSE()
        FIND_LIBRARY( RA_ENGINE_LIB
            NAMES radiumEngine
            PATHS ${RADIUM_ROOT}/${CMAKE_BUILD_TYPE}/lib
            )
    ENDIF()

    IF (TARGET radiumGuiBase)
        set (RA_GUIBASE_LIB radiumGuiBase)
    ELSE()
        FIND_LIBRARY ( RA_GUIBASE_LIB
            NAMES radiumGuiBase
            PATHS ${RADIUM_ROOT}/${CMAKE_BUILD_TYPE}/lib
            )
    ENDIF()

    SET ( Radium_FOUND TRUE )
    IF ( RA_CORE_LIB AND RA_ENGINE_LIB AND RA_GUIBASE_LIB)
        SET (RADIUM_LIBRARIES "${RA_CORE_LIB} ${RA_ENGINE_LIB} ${RA_GUIBASE_LIB}")
        SET ( Radium_Libs_FOUND TRUE)
    ENDIF ( RA_CORE_LIB AND RA_ENGINE_LIB AND RA_GUIBASE_LIB)
ENDIF( RADIUM_ROOT)

IF ( Radium_FOUND )
    IF(NOT Radium_FIND_QUIETLY)
        MESSAGE ( STATUS "Found Radium Egine root dir: ${RADIUM_ROOT}")
    ENDIF(NOT Radium_FIND_QUIETLY)
    IF (NOT Radium_Libs_FOUND)
        MESSAGE( WARNING "Could not find Radium libraries. You must compile them first")
    ENDIF (NOT Radium_Libs_FOUND)
ELSE(Radium_FOUND)
    IF(Radium_FIND_REQUIRED)
        MESSAGE( FATAL_ERROR "Could not find Radium Engine root dir")
    ENDIF(Radium_FIND_REQUIRED)
ENDIF(Radium_FOUND)
