execute_process( COMMAND git submodule init )
execute_process( COMMAND git submodule update )

if ((${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang"))
    add_definitions( -fexceptions -frtti -w )
elseif ( UNIX OR MINGW )
    add_definitions( -fexceptions -frtti -w )
    if ( MINGW )
        add_definitions (-D_USE_MATH_DEFINES)
    endif()
elseif (MSVC)
    set( CMAKE_DEBUG_POSTFIX "" )
    string (REGEX REPLACE "/EHs-c-" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string (REGEX REPLACE "/GR-" ""    CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    add_definitions( "/EHsc /GR /w")
endif()

set( ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL
    "If the supplementary tools for Assimp are built in addition to the library." )
set( ASSIMP_BUILD_TESTS OFF CACHE BOOL
    "If the test suite for Assimp is built in addition to the library." )
set( ASSIMP_BUILD_SAMPLES OFF CACHE BOOL
    "If the official samples are built as well (needs Glut)." )

add_subdirectory( Assimp )
