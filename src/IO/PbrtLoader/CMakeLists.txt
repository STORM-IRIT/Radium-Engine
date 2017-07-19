include_directories(
        .
        ${RADIUM_INCLUDE_DIRS}
)

file( GLOB_RECURSE pbrt_sources *.cpp *.c )
file( GLOB_RECURSE pbrt_headers *.hpp *.h )
file( GLOB_RECURSE pbrt_inlines *.inl )

set (pbrt_sources ${pbrt_sources} PARENT_SCOPE)
set (pbrt_headers ${pbrt_headers} PARENT_SCOPE)
set (pbrt_inlines ${pbrt_inlines} PARENT_SCOPE)

set( RADIUM_IO_IS_INTERFACE FALSE PARENT_SCOPE )
