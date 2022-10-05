cmake_minimum_required(VERSION 3.6)

project(radiumdataflow-external VERSION 1.0.0)

include(ExternalProject)
include(ExternalInclude)

# force installing by default all the external projects
set_property(DIRECTORY PROPERTY EP_STEP_TARGETS install)

# Add fPIC for all dependencies
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
string(REPLACE ";" "" indent_string "${CMAKE_MESSAGE_INDENT}")
set(indent_string "${indent_string}--")

add_custom_target(DataflowExternals ALL)

if(NOT DEFINED stduuid_DIR)
    check_externals_prerequisite()
    status_message("[DataflowExternal]" "stduuid" "remote git")
    ExternalProject_Add(
        stduuid
        GIT_TAG 3afe7193facd5d674de709fccc44d5055e144d7a
        GIT_REPOSITORY https://github.com/mariusbancila/stduuid
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        PATCH_COMMAND git reset --hard && git apply -v --ignore-whitespace
                      "${CMAKE_CURRENT_LIST_DIR}/patches/stduuid.patch"
        INSTALL_DIR "${CMAKE_INSTALL_PREFIX}"
        CMAKE_ARGS ${RADIUM_EXTERNAL_CMAKE_OPTIONS} -DUUID_BUILD_TESTS=OFF -DUUID_ENABLE_INSTALL=ON
                   "-DCMAKE_MESSAGE_INDENT=${indent_string}\;"
    )
    add_dependencies(DataflowExternals stduuid)
else()
    status_message("" "stduuid" ${stduuid_DIR})
endif()

if(NOT DEFINED RadiumNodeEditor_DIR)
    check_externals_prerequisite()
    status_message("[DataflowExternal]" "RadiumNodeEditor" "remote git")
    ExternalProject_Add(
        RadiumNodeEditor
        GIT_REPOSITORY https://github.com/MathiasPaulin/RadiumQtNodeEditor.git
        GIT_TAG main
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        INSTALL_DIR "${CMAKE_INSTALL_PREFIX}"
        CMAKE_ARGS ${RADIUM_EXTERNAL_CMAKE_OPTIONS} "-DCMAKE_MESSAGE_INDENT=${indent_string}\;"
    )
    add_dependencies(DataflowExternals RadiumNodeEditor)
else()
    status_message("" "RadiumNodeEditor" ${RadiumNodeEditor_DIR})
endif()
