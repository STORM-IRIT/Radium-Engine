cmake_minimum_required(VERSION 3.16)

project(radiumgui-external VERSION 1.0.0)

include(ExternalProject)
include(ExternalInclude)

macro(add_dependency_if_compiled target dep)
    if(TARGET ${dep})
        add_dependencies(${target} ${dep})
    endif()
endmacro()

# force installing by default all the external projects
set_property(DIRECTORY PROPERTY EP_STEP_TARGETS install)

# Add fPIC for all dependencies
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

string(REPLACE ";" "" indent_string "${CMAKE_MESSAGE_INDENT}")
set(indent_string "${indent_string}--")

add_custom_target(GuiExternals ALL)

# ------------------------------------------------------------------------------

if(NOT DEFINED PowerSlider_DIR)
    check_externals_prerequisite()
    status_message("[GuiExternal]" "PowerSlider" "remote git")
    ExternalProject_Add(
        PowerSlider
        GIT_REPOSITORY https://github.com/dlyr/PowerSlider.git
        GIT_TAG origin/master
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        INSTALL_DIR "${CMAKE_INSTALL_PREFIX}"
        CMAKE_ARGS ${RADIUM_EXTERNAL_CMAKE_OPTIONS} -DBUILD_DESIGNER_PLUGIN=OFF
                   -DBUILD_EXAMPLE_APP=OFF -DCMAKE_MESSAGE_INDENT=${indent_string}\;
    )
    add_dependencies(GuiExternals PowerSlider)
else()
    status_message("" "PowerSlider" "${PowerSlider_DIR}")
endif()
