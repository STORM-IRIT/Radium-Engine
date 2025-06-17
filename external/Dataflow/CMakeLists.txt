project(radiumdataflow-external VERSION 1.0.0)

include(ExternalProject)
include(ExternalInclude)

list(APPEND CMAKE_MESSAGE_INDENT "[Dataflow] ")
string(REPLACE ";" "" indent_string "${CMAKE_MESSAGE_INDENT}")
set(indent_string "${indent_string}--")

# force installing by default all the external projects
set_property(DIRECTORY PROPERTY EP_STEP_TARGETS install)

# Add fPIC for all dependencies
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_custom_target(DataflowExternals ALL)

if(NOT DEFINED QtNodes_DIR)
    check_externals_prerequisite()
    status_message("QtNodes" "remote git")
    ExternalProject_Add(
        QtNodes
        GIT_REPOSITORY https://github.com/paceholder/nodeeditor.git
        GIT_TAG 34b6f81
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        INSTALL_DIR "${CMAKE_INSTALL_PREFIX}"
        CMAKE_ARGS ${RADIUM_EXTERNAL_CMAKE_OPTIONS} -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
                   "-DCMAKE_MESSAGE_INDENT=${indent_string}\;"
    )
    set_external_dir(QtNodes "lib/cmake/QtNodes/")
    add_dependencies(DataflowExternals QtNodes)
else()
    status_message("QtNodes" ${QtNodes_DIR})
endif()
