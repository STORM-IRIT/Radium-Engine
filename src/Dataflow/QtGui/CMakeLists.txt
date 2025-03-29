set(ra_dataflowqtgui_target DataflowQtGui)

message_prefix_push("${ra_dataflowqtgui_target}")

project(${ra_dataflowqtgui_target} LANGUAGES CXX VERSION ${Radium_VERSION})

# Qt utility functions
include(QtFunctions)

# need to find qt before QtNodes, otherwise got error Some (but not all) targets in this export set
# were already defined. Targets Defined: Qt::Core Targets not yet defined: Qt::CorePrivate
find_qt_package(COMPONENTS Core Widgets OpenGL Xml REQUIRED)

find_package(QtNodes REQUIRED NO_DEFAULT_PATH)
find_package(PowerSlider REQUIRED)

include(filelist.cmake)

# Find Qt packages
set(QT_DEFAULT_MAJOR_VERSION ${QT_DEFAULT_MAJOR_VERSION} PARENT_SCOPE)
set(Qt_LIBRARIES Qt::Core Qt::Widgets Qt::OpenGL Qt::Xml)
set(CMAKE_AUTOMOC_PATH_PREFIX ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/")
set(CMAKE_AUTOUIC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Moc
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# configure library
add_library(
    ${ra_dataflowqtgui_target} SHARED ${dataflow_qtgui_sources} ${dataflow_qtgui_headers}
                                      ${dataflow_qtgui_resources}
)

target_compile_definitions(${ra_dataflowqtgui_target} PRIVATE RA_DATAFLOW_GUI_EXPORTS)

target_compile_options(${ra_dataflowqtgui_target} PUBLIC ${RA_DEFAULT_COMPILE_OPTIONS})

target_include_directories(${ra_dataflowqtgui_target} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

add_dependencies(${ra_dataflowqtgui_target} DataflowCore Gui)

target_link_libraries(
    ${ra_dataflowqtgui_target} PUBLIC ${Qt_LIBRARIES} DataflowCore Gui PUBLIC QtNodes::QtNodes
    PRIVATE PowerSlider::PowerSlider
)

message(STATUS "Configuring library ${ra_dataflowqtgui_target} with standard settings")
configure_radium_target(${ra_dataflowqtgui_target})
# configure the library only. The package is a sub-package and should be configured independently
configure_radium_library(
    TARGET ${ra_dataflowqtgui_target} COMPONENT TARGET_DIR "Dataflow/QtGui"
    FILES "${dataflow_qtgui_headers}"
)
# Generate cmake package
configure_radium_package(
    NAME ${ra_dataflowqtgui_target} PACKAGE_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    PACKAGE_DIR "lib/cmake/Radium/${ra_dataflowqtgui_target}" NAME_PREFIX "Radium"
)

set(RADIUM_COMPONENTS ${RADIUM_COMPONENTS} ${ra_dataflowqtgui_target} PARENT_SCOPE)

if(RADIUM_ENABLE_PCH)
    target_precompile_headers(${ra_dataflowqtgui_target} PRIVATE pch.hpp)
endif()

message_prefix_pop()
