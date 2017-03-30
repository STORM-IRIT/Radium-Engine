
find_package(Radium REQUIRED)

message(STATUS "Building plugins with radium libs : ${RADIUM_LIBRARIES} ")

set( CMAKE_DEBUG_POSTFIX "")

set( PLUGIN_OUTPUT_PATH "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Plugins")

#add_subdirectory( Dummy )
add_subdirectory( FancyMesh )
add_subdirectory( Animation )
add_subdirectory( Skinning )
