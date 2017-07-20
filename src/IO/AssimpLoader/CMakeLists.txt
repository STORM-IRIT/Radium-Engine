include_directories(
        .
        ${RADIUM_INCLUDE_DIRS}
)

file( GLOB_RECURSE assimp_sources *.cpp *.c )
file( GLOB_RECURSE assimp_headers *.hpp *.h )
file( GLOB_RECURSE assimp_inlines *.inl )

set (assimp_sources ${assimp_sources} PARENT_SCOPE)
set (assimp_headers ${assimp_headers} PARENT_SCOPE)
set (assimp_inlines ${assimp_inlines} PARENT_SCOPE)

set( RADIUM_IO_IS_INTERFACE FALSE PARENT_SCOPE )
