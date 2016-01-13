execute_process( COMMAND git submodule init
                 COMMAND git submodule update )

if ( UNIX )
    add_definitions( -fexceptions -frtti -Wno-ignored-qualifiers -Wno-unused-but-set-variable )
endif( UNIX )

set( ASSIMP_BUILD_ASSIMP_TOOLS 0 )
set( ASSIMP_BUILD_TESTS 0 )
set( ASSIMP_BUILD_SAMPLES 0 )

add_subdirectory( Assimp )
