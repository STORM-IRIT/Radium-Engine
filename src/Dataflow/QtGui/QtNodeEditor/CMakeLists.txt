cmake_minimum_required(VERSION 3.2)
# version 3.4 is required as other do not work with C++14 and clang

project(NodeEditor CXX)

set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)

get_directory_property(_has_parent PARENT_DIRECTORY)
if(_has_parent)
    set(is_root_project OFF)
else()
    set(is_root_project ON)
endif()

set(NE_DEVELOPER_DEFAULTS "${is_root_project}"
    CACHE BOOL "Turns on default settings for development of NodeEditor"
)

option(BUILD_SHARED_LIBS "Build as shared library" ON)
option(BUILD_DEBUG_POSTFIX_D "Append d suffix to debug libraries" OFF)

enable_testing()

if(NE_DEVELOPER_DEFAULTS)
    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_EXTENSIONS OFF)

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
endif()

if(BUILD_DEBUG_POSTFIX_D)
    set(CMAKE_DEBUG_POSTFIX d)
endif()

# include(${CMAKE_CURRENT_LIST_DIR}/../../../../cmake/QtFunctions.cmake) Qt utility functions
include(QtFunctions)

# Find the QtWidgets library
find_qt_package(COMPONENTS Core Widgets Gui OpenGL REQUIRED)

qt_add_resources(RESOURCES ./resources/resources.qrc)

# Unfortunately, as we have a split include/src, AUTOMOC doesn't work. We'll have to manually
# specify some files
set(CMAKE_AUTOMOC ON)

set(CPP_SOURCE_FILES
    src/Connection.cpp
    src/ConnectionBlurEffect.cpp
    src/ConnectionGeometry.cpp
    src/ConnectionGraphicsObject.cpp
    src/ConnectionPainter.cpp
    src/ConnectionState.cpp
    src/ConnectionStyle.cpp
    src/DataModelRegistry.cpp
    src/FlowScene.cpp
    src/FlowView.cpp
    src/FlowViewStyle.cpp
    src/Node.cpp
    src/NodeConnectionInteraction.cpp
    src/NodeDataModel.cpp
    src/NodeGeometry.cpp
    src/NodeGraphicsObject.cpp
    src/NodePainter.cpp
    src/NodeState.cpp
    src/NodeStyle.cpp
    src/Properties.cpp
    src/StyleCollection.cpp
)

# If we want to give the option to build a static library, set BUILD_SHARED_LIBS option to OFF
add_library(NodeEditor ${CPP_SOURCE_FILES} ${RESOURCES})
add_library(NodeEditor::NodeEditor ALIAS NodeEditor)

target_include_directories(
    NodeEditor
    PUBLIC $<INSTALL_INTERFACE:include> $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/nodes/internal>
)

target_link_libraries(NodeEditor PUBLIC Qt::Core Qt::Widgets Qt::Gui Qt::OpenGL)

target_compile_definitions(
    NodeEditor
    PUBLIC ${QtWidgets_DEFINITIONS} NODE_EDITOR_SHARED
    PRIVATE NODE_EDITOR_EXPORTS
            # NODE_DEBUG_DRAWING
            QT_NO_KEYWORDS
)

target_compile_options(
    NodeEditor PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/W4 /wd4127 /EHsc> $<$<CXX_COMPILER_ID:GNU>:-Wall
                       -Wextra> $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra>
)

target_compile_features(NodeEditor PUBLIC cxx_generic_lambdas # Require C++14
)

set_target_properties(
    NodeEditor
    PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib LIBRARY_OUTPUT_DIRECTORY
                                                                ${CMAKE_BINARY_DIR}/lib
               RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

#
# Moc
#

file(GLOB_RECURSE HEADERS_TO_MOC include/nodes/internal/*.hpp)

qt_wrap_cpp(
    nodes_moc ${HEADERS_TO_MOC} TARGET NodeEditor OPTIONS --no-notes # Don't display a note for the
                                                                     # headers which don't produce a
                                                                     # moc_*.cpp
)

target_sources(NodeEditor PRIVATE ${nodes_moc})

# ##################################################################################################
# Installation
#

include(GNUInstallDirs)

set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/NodeEditor)

install(TARGETS NodeEditor EXPORT NodeEditorTargets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT NodeEditorTargets FILE NodeEditorTargets.cmake NAMESPACE NodeEditor::
        DESTINATION ${INSTALL_CONFIGDIR}
)

export(TARGETS NodeEditor NAMESPACE NodeEditor:: FILE NodeEditorTargets.cmake)

include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_CURRENT_LIST_DIR}/cmake/NodeEditorConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/NodeEditorConfig.cmake INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

install(FILES ${CMAKE_CURRENT_LIST_DIR}/cmake/QtFunctions.cmake DESTINATION ${INSTALL_CONFIGDIR})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/NodeEditorConfig.cmake DESTINATION ${INSTALL_CONFIGDIR})
