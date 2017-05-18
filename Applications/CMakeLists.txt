
# Painty project
# Radium

# Buggy accumulation in CMAKE_MODULE_PATH
#set (CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH} ../cmake)

find_package(Radium REQUIRED)
# Add Painty source code

add_subdirectory(MainApplication)
add_subdirectory(Test0)
#add_subdirectory(OpenGLExperiment)
