
# This Plugin uses an external helper library (DummyLibrary).
# This library can be searched and used in its build tree or in an installed one.
if (NOT TARGET DummyLibrary)
    message(STATUS "Searching for package DummyLibrary")
    find_package(DummyLibrary REQUIRED)
endif ()
#once found (or used from the same buildtree than this project, the helper library has name DummyLibrary::DummyLibrary

# Poupulate the plugin by its sources
set(sources
    DummyParasitePlugin
    )
set(headers
    DummyParasitePlugin
    )
# Our library project uses these sources and headers.
add_library(
    ${PROJECT_NAME} SHARED
    ${sources}
    ${headers}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_EXPORTS)

target_link_libraries(
    ${PROJECT_NAME}
    Radium::Core
    Radium::Engine
    Radium::PluginBase
    ${Qt5_LIBRARIES}
    DummyLibrary::DummyLibrary
)

#-----------------------------------------------------------------------------------
# Radium plugin configuration : manage package and installation
message(STATUS "Configuring the plugin ${PROJECT_NAME}")
configure_radium_plugin(
    NAME ${PROJECT_NAME}
    HELPER_LIBS DummyLibrary::DummyLibrary
)
