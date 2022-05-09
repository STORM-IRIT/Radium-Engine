cmake_minimum_required(VERSION 3.13)
if(APPLE)
    cmake_policy(SET CMP0042 NEW)
endif(APPLE)

set(ra_headless_target Headless)
list(APPEND CMAKE_MESSAGE_INDENT "[${ra_headless_target}] ")
project(${ra_headless_target} LANGUAGES CXX VERSION ${Radium_VERSION})
# -----------------------------------------------------------------------------------
# lib headless configuration
# -----------------------------------------------------------------------------------
# find required dependencies
find_package(glfw3 3.2 QUIET)
if(NOT glfw3_FOUND)
    # do not configure Radium headless if dependency is not found
    message(WARNING "glfw3 package not found. Headless component is disabled")
else()
    # configure the headless component for build and install as component of the Radium package
    message(STATUS "Headless component is enabled")
    # set source files
    set(CLI11_headers
        CLI/App.hpp
        CLI/CLI.hpp
        CLI/Config.hpp
        CLI/ConfigFwd.hpp
        CLI/Error.hpp
        CLI/Formatter.hpp
        CLI/FormatterFwd.hpp
        CLI/Macros.hpp
        CLI/Option.hpp
        CLI/Split.hpp
        CLI/StringTools.hpp
        CLI/Timer.hpp
        CLI/TypeTools.hpp
        CLI/Validators.hpp
        CLI/Version.hpp
    )

    set(${ra_headless_target}_sources CLIBaseApplication.cpp CLIViewer.cpp
                                      OpenGLContext/OpenGLContext.cpp
    )

    set(${ra_headless_target}_headers ${CLI11_headers} CLIBaseApplication.hpp CLIViewer.hpp
                                      RaHeadless.hpp OpenGLContext/OpenGLContext.hpp
    )

    set(${ra_headless_target}_inlines CLIBaseApplication.inl CLIViewer.inl)

    add_library(
        ${ra_headless_target} SHARED
        ${${ra_headless_target}_sources} ${${ra_headless_target}_headers}
        ${${ra_headless_target}_inlines}
    )
    add_dependencies(${ra_headless_target} Core Engine IO)
    target_link_libraries(
        ${ra_headless_target} PUBLIC Radium::Core Radium::Engine Radium::IO PRIVATE glfw
    )
    target_compile_options(${ra_headless_target} PUBLIC ${RA_DEFAULT_COMPILE_OPTIONS})
    target_compile_definitions(
        ${ra_headless_target} PRIVATE ${ra_headless_target}_EXPORTS GLFW_INCLUDE_NONE
    )
    configure_radium_target(${ra_headless_target})

    # configure the component "Headless" of packages "Radium"
    set(installed_headers "${${ra_headless_target}_headers};${${ra_headless_target}_inlines}")
    configure_radium_library(
        TARGET ${ra_headless_target} COMPONENT
        PACKAGE_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in FILES "${installed_headers}"
    )
    set(RADIUM_COMPONENTS ${RADIUM_COMPONENTS} ${ra_headless_target} PARENT_SCOPE)
endif()
list(REMOVE_AT CMAKE_MESSAGE_INDENT -1)
