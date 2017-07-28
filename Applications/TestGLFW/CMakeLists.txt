set(app_target testGLFW)

find_package(glfw REQUIRED)
find_package(nanogui REQUIRED)

# Access to Radium headers and declarations/defintions
include_directories(
    .
    ${RADIUM_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIR}
    ${NANOGUI_INCLUDE_DIR}
)

# Get files
file( GLOB file_sources *.cpp *.c )
file( GLOB file_headers *.hpp *.h )

# Generate an executable
add_executable( ${app_target} ${file_sources} ${file_headers} )

add_dependencies( ${app_target} radiumEngine radiumCore radium_assets)
target_compile_options(${app_target} PRIVATE -fexceptions)


if(UNIX)
    find_package(X11 REQUIRED)
    set (ADDITIONAL_LIBS dl X11 Xrandr Xinerama Xxf86vm Xcursor pthread)
else()
    set(ADDITIONAL_LIBS "")
endif()

# Link libraries
target_link_libraries( ${app_target} # target
    radiumEngine
    radiumIO
    radiumCore
    ${GLBINDING_LIBRARIES}           # Radium dep
    ${NANOGUI_LIBRARY}
    ${GLFW_LIBRARY}
    ${ADDITIONAL_LIBS}
)

if (MSVC)
    set_property( TARGET ${app_target} PROPERTY IMPORTED_LOCATION "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/bin" )
endif(MSVC)
