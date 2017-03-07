
find_package(Radium REQUIRED)


set( CMAKE_DEBUG_POSTFIX "")
if (APPLE)
    FILE(GLOB RADIUM_LIBRARIES "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/*.dylib" )
elseif(UNIX)
    FILE(GLOB RADIUM_LIBRARIES "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/*.so" )
elseif (MINGW)
    FILE(GLOB RADIUM_LIBRARIES "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/*.dll.a")
elseif(MSVC)
    FILE(GLOB RADIUM_LIBRARIES "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/*.lib" )
endif()
set( PLUGIN_OUTPUT_PATH "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Plugins")

#add_subdirectory( Dummy )
add_subdirectory( FancyMesh )
add_subdirectory( Animation )
add_subdirectory( Skinning )
