set(app_target test0)

# Qt stuff //////////////////////

find_package(OpenGL     REQUIRED)
find_package(Qt5Core    REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL  REQUIRED)

set( Qt5_LIBRARIES
     ${Qt5Core_LIBRARIES}
     ${Qt5Widgets_LIBRARIES}
     ${Qt5OpenGL_LIBRARIES} )

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# ///////////////////////////////

# Flags
set( CMAKE_CXX_FLAGS "-std=c++14" )

# Access to Radium headers and declarations/defintions
include_directories(
    .
    ${RADIUM_LIB_SRC}
    ${RADIUM_INC_EIGEN}
    ${RADIUM_INC_ASSIMP}
)



# Libs include
link_directories(
    ${RADIUM_LIB_DIR}
)


# Get files
#file( GLOB file_sources *.cpp *.c )
#file( GLOB file_headers *.hpp *.h )
file( GLOB file_sources main.cpp )
file( GLOB file_headers          )


# Generate an executable
add_executable( ${app_target} ${file_sources} ${file_headers} )



# Link good libraries
target_link_libraries( ${app_target} # target
    ${RADIUM_LIB_CORE}              # core
    ${RADIUM_LIB_ENGINE}            # engine
    ${RADIUM_LIB_GUIBASE}           # gui
    ${Qt5_LIBRARIES}                # the Qt beast
)
