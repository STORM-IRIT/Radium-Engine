# add only support dependant includes
# ${RADIUM_SRC_DIR} is always added by the parent
# AssimpLoader need access to ${ASSIMP_INCLUDE_DIR}
include_directories(
        ${ASSIMP_INCLUDE_DIR}
)

file( GLOB_RECURSE assimp_sources *.cpp *.c )
file( GLOB_RECURSE assimp_headers *.hpp *.h )
file( GLOB_RECURSE assimp_inlines *.inl )

set( assimp_sources ${assimp_sources} PARENT_SCOPE )
set( assimp_headers ${assimp_headers} PARENT_SCOPE )
set( assimp_inlines ${assimp_inlines} PARENT_SCOPE )

set( RADIUM_IO_IS_INTERFACE FALSE PARENT_SCOPE )
