execute_process( COMMAND git submodule init )
execute_process( COMMAND git submodule update )

if (("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))
    add_definitions( -fexceptions -frtti -Wno-ignored-qualifiers )
elseif ( UNIX )
    add_definitions( -fexceptions -frtti -Wno-ignored-qualifiers -Wno-unused-but-set-variable )
endif()

set( ASSIMP_BUILD_ASSIMP_TOOLS 0 )
set( ASSIMP_BUILD_TESTS 0 )
set( ASSIMP_BUILD_SAMPLES 0 )

add_subdirectory( Assimp )
