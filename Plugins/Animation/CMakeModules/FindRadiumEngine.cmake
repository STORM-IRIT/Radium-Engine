find_path(
    RadiumEngine_INCLUDE_DIR Radium/RadiumEngine.hpp
    ${RadiumEngine_ROOT}/include
    /usr/include
    /usr/local/include
    /usr/local/RadiumEngine/include
)

find_library(
    RadiumEngine_CORE_LIBRARY
    NAMES radiumCore
    ${RadiumEngine_ROOT}/lib
    /usr/lib
    /usr/local/lib
    /usr/local/RadiumEngine/lib
)

find_library(
    RadiumEngine_ENGINE_LIBRARY
    NAMES radiumEngine
    ${RadiumEngine_ROOT}/lib
    /usr/lib
    /usr/local/lib
    /usr/local/RadiumEngine/lib
)

if( RadiumEngine_INCLUDE_DIR AND
    RadiumEngine_CORE_LIBRARY AND RadiumEngine_ENGINE_LIBRARY )

    set( RadiumEngine_FOUND TRUE )
    set( RadiumEngine_LIBRARIES
         "${RadiumEngine_CORE_LIBRARY};${RadiumEngine_ENGINE_LIBRARY}" )

endif()

if ( NOT RadiumEngine_FIND_QUIETLY )
    set( RadiumEngine_FIND_QUIETLY FALSE )
endif()

if ( RadiumEngine_FOUND )
    if ( NOT RadiumEngine_FIND_QUIETLY )
        message( STATUS "Found RadiumEngine: ${RadiumEngine_INCLUDE_DIR} ${RadiumEngine_LIBRARIES}")
    endif()
else()
    if ( RadiumEngine_FIND_REQUIRED )
        message( FATAL_ERROR "Could not find RadiumEngine" )
    endif()
endif()
