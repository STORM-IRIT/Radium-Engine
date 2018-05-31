set(app_target helloRadium)

# Qt stuff //////////////////////

find_package(OpenGL     REQUIRED)
find_package(Qt5 COMPONENTS Core Widgets OpenGL REQUIRED)

set( Qt5_LIBRARIES
     Qt5::Core
     Qt5::Widgets
     Qt5::OpenGL )

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} VERSION_GREATER "3.9")
    cmake_policy(SET CMP0071 NEW)
endif()

# ///////////////////////////////

# Access to Radium headers and declarations/defintions
include_directories(
    .
    ${RADIUM_INCLUDE_DIRS}
)



# Get files
file( GLOB file_sources *.cpp *.c )
file( GLOB file_headers *.hpp *.h )


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
    set_property( TARGET ${app_target} PROPERTY IMPORTED_LOCATION "${RADIUM_BINARY_OUTPUT_PATH}" )
endif(MSVC)
