set(app_target testGLFW)

#find_package(glfw3 REQUIRED)

set (GLFW_LIB "/usr/lib/x86_64-linux-gnu/libglfw.so")


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

add_dependencies( ${app_target} radiumEngine radiumCore radium_assets)


# Link good libraries
target_link_libraries( ${app_target} # target
    ${RADIUM_LIBRARIES}              # Radium libs
    ${GLBINDING_LIBRARIES}           # Radium dep
    ${GLFW_LIB}
)

if (MSVC)
    set_property( TARGET ${app_target} PROPERTY IMPORTED_LOCATION "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/bin" )
endif(MSVC)
