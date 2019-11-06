#-------------------------------------------------------------------------------
# exampleApp executables setup

project(RadiumExampleApps)

# short convenience target from cmake/cleanCpppExtensions.cpp
add_custom_target(test-run)
add_custom_target(${PROJECT_NAME})

# These variable are normally set by the find_package(Radium) for client applications
set(RADIUM_ROOT_DIR "${CMAKE_INSTALL_PREFIX}/")
set(RADIUM_RESOURCES_DIR "${CMAKE_INSTALL_PREFIX}/Resources")
set(RADIUM_PLUGINS_DIR "${CMAKE_INSTALL_PREFIX}/Plugins")

include(application_setup)

add_executable(CoreExampleApp
    CoreExampleApp/CoreExampleApp.cpp)
target_link_libraries (CoreExampleApp PUBLIC Radium::Core)
configure_radium_app_install(NAME CoreExampleApp)

#if (RADIUM_GENERATE_LIB_ENGINE)
#    find_package(Qt5 COMPONENTS Core Widgets OpenGL Xml REQUIRED)
#    find_package(OpenGL REQUIRED)
#    set(CMAKE_AUTOMOC ON)
#    add_executable( EngineExampleApp MACOSX_BUNDLE
#            EngineExampleApp/main.cpp
#            EngineExampleApp/minimalapp.cpp
#            EngineExampleApp/minimalapp.hpp
#            EngineExampleApp/minimalradium.cpp
#            EngineExampleApp/minimalradium.hpp)
#    target_include_directories(EngineExampleApp PRIVATE EngineExampleApp)
#    target_link_libraries(EngineExampleApp PUBLIC
#            Radium::Core Radium::Engine Radium::GuiBase
#            Qt5::Core Qt5::Widgets Qt5::OpenGL Qt5::Xml
#            OpenGL::GL)
#    message(STATUS " Installing EngineExampleApp into ${CMAKE_INSTALL_PREFIX}")
#    configure_radium_app_install(NAME EngineExampleApp)
#endif ()

