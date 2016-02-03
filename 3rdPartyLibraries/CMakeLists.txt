execute_process( COMMAND git submodule init )
execute_process( COMMAND git submodule update )

if (("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))
    add_definitions( -fexceptions -frtti -Wno-ignored-qualifiers )
elseif ( UNIX )
    add_definitions( -fexceptions -frtti -Wno-ignored-qualifiers -Wno-unused-but-set-variable )
endif()

set( ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL 
    "If the supplementary tools for Assimp are built in addition to the library." )
set( ASSIMP_BUILD_TESTS OFF CACHE BOOL
    "If the test suite for Assimp is built in addition to the library." )
set( ASSIMP_BUILD_SAMPLES OFF CACHE BOOL 
    "If the official samples are built as well (needs Glut)." )

add_subdirectory( Assimp )
