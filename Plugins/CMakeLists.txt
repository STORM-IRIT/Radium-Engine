
find_package(Radium REQUIRED)

message(STATUS "Building plugins with radium libs : ${RADIUM_LIBRARIES} ")

set( CMAKE_DEBUG_POSTFIX "")

#add_subdirectory( Dummy )
add_subdirectory( FancyMesh )
add_subdirectory( Animation )
add_subdirectory( Skinning )
add_subdirectory( AssimpLoader )
