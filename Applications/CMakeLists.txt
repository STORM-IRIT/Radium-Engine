# Settings
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../bin)
set(EXECUTABLE_OUTPUT_PATH         ${CMAKE_CURRENT_SOURCE_DIR}/../bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../lib)

# Painty project
# Radium

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ../CMakeModules)
find_package(Radium REQUIRED)

# Add Painty source code

add_subdirectory(MainApplication)
add_subdirectory(Test0)
