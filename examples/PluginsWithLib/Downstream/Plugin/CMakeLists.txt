message(STATUS "[Plugins sharing lib] : configure plugin ${PROJECT_NAME}")
# This Plugin uses an external helper library (UpstreamLibrary). This library can be searched and
# used in its build tree or from an installation one.
find_package(UpstreamLibrary REQUIRED)
# once found (or used from the same buildtree than this project), the helper library has name
# RadiumDemo::UpstreamLibrary

# search for Qt dependency for the plugin
find_qt_package(COMPONENTS Core Widgets REQUIRED)
set(Qt_LIBRARIES Qt::Core Qt::Widgets)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
include_directories(${CMAKE_CURRENT_BINARY_DIR} # Moc
)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Populate the plugin by its sources
set(sources PluginB.cpp)
set(headers PluginB.hpp)
# Our library project uses these sources and headers.
add_library(${PROJECT_NAME} SHARED ${sources} ${headers})

target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_EXPORTS)

target_link_libraries(
    ${PROJECT_NAME} Radium::Core Radium::Engine Radium::PluginBase ${Qt_LIBRARIES}
    RadiumExamples::UpstreamLibrary
)

# -----------------------------------------------------------------------------------
# Radium plugin configuration : manage package and installation
message(STATUS "Configuring the plugin ${PROJECT_NAME}")
configure_radium_plugin(NAME ${PROJECT_NAME} HELPER_LIBS RadiumExamples::UpstreamLibrary)
