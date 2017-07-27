set(app_target OpenGLExperiment)

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

find_package(GLBinding REQUIRED)
find_package(GLM REQUIRED)


# Access to Radium headers and declarations/defintions
include_directories(
    .
    ${RADIUM_INCLUDE_DIR}
    ${EIGEN_INCLUDE_DIR}
    ${ASSIMP_INCLUDE_DIR}
)



# Get files
#file( GLOB file_sources *.cpp *.c )
#file( GLOB file_headers *.hpp *.h )
file( GLOB file_sources main.cpp )
file( GLOB file_headers          )


# Generate an executable
add_executable( ${app_target} ${file_sources} ${file_headers} )

add_dependencies( ${app_target}  radium_assets)


# Link good libraries
target_link_libraries( ${app_target} # target

    ${RA_GUIBASE_LIB}           # gui
    ${RA_ENGINE_LIB}            # engine
    ${RA_CORE_LIB}              # core
    ${Qt5_LIBRARIES}                # the Qt beast
)
if (MSVC)
    set_property( TARGET ${app_target} PROPERTY IMPORTED_LOCATION "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/bin" )
endif(MSVC)
