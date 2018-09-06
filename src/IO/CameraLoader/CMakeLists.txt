include_directories(
        .
        ${RADIUM_INCLUDE_DIRS}
)

file( GLOB_RECURSE cameraloader_sources *.cpp *.c )
file( GLOB_RECURSE cameraloader_headers *.hpp *.h )
file( GLOB_RECURSE cameraloader_inlines *.inl )

set (cameraloader_sources ${cameraloader_sources} ${cameraloaderlib_sources} PARENT_SCOPE)
set (cameraloader_headers ${cameraloader_headers} PARENT_SCOPE)
set (cameraloader_inlines ${cameraloader_inlines} PARENT_SCOPE)

