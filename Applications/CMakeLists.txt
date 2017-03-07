
# Painty project
# Radium

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ../CMakeModules)
find_package(Radium REQUIRED)

# Add Painty source code

add_subdirectory(MainApplication)
add_subdirectory(Test0)
