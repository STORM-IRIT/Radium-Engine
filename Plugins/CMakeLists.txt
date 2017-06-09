cmake_minimum_required(VERSION 2.8)
find_package(Radium REQUIRED)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)
set( PLUGIN_OUTPUT_PATH "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Plugins")
# use nothing as debug postfix to avoid creating duplicate plugins when switching builds
set( CMAKE_DEBUG_POSTFIX "")
#add_subdirectory(ProgressiveMesh)
add_subdirectory(FancyMesh)
add_subdirectory(MeshContact)

