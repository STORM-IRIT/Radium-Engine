if ( UNIX )
    add_definitions( -fexceptions -frtti -Wno-ignored-qualifiers -Wno-unused-but-set-variable )
endif( UNIX )

set( ASSIMP_BUILD_TESTS OFF )

add_subdirectory( Assimp )
