# add only support dependant includes
# ${RADIUM_SRC_DIR} is always added by the parent
# TinyPlyLoader does not need specific include_directories
# include_directories( )

file( GLOB_RECURSE tinyply_sources *.cpp *.c )
file( GLOB_RECURSE tinyply_headers *.hpp *.h )
file( GLOB_RECURSE tinyply_inlines *.inl )

set( tinyply_sources ${tinyply_sources} ${tinyplylib_sources} PARENT_SCOPE )
set( tinyply_headers ${tinyply_headers} PARENT_SCOPE )
set( tinyply_inlines ${tinyply_inlines} PARENT_SCOPE )

set( RADIUM_IO_IS_INTERFACE FALSE PARENT_SCOPE )
