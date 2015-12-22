set( RADIUM_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/src" )
set( RADIUM_LIBRARIES "${CMAKE_SOURCE_DIR}/lib/libradiumCore.so;${CMAKE_SOURCE_DIR}/lib/libradiumEngine.so" )
set( PLUGIN_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/Plugins/bin" )

#add_subdirectory( Dummy )
add_subdirectory( FancyMesh )
