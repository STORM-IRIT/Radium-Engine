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

# Access to Radium headers and declarations/defintions
include_directories(
    .
    ${RADIUM_INCLUDE_DIR}
)



# Get files
file( GLOB file_sources *.cpp *.c )
file( GLOB file_headers *.hpp *.h )
#file( GLOB file_sources main.cpp minimalradium.cpp)
#file( GLOB file_headers minimalradium.hpp)


# Generate an executable
add_executable( ${app_target} ${file_sources} ${file_headers} )

add_dependencies( ${app_target} radiumEngine radiumGuiBase radiumCore radium_assets)


# Link good libraries
target_link_libraries( ${app_target} # target
    ${RADIUM_LIBRARIES}              # Radium libs
    ${GLBINDING_LIBRARIES}           # Radium dep
    ${Qt5_LIBRARIES}                 # the Qt beast
)

if (MSVC)
    set_property( TARGET ${app_target} PROPERTY IMPORTED_LOCATION "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/bin" )
endif(MSVC)
