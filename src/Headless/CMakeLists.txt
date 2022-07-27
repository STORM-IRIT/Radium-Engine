cmake_minimum_required(VERSION 3.16)
if(APPLE)
    cmake_policy(SET CMP0042 NEW)
endif(APPLE)

set(RA_HEADLESS_TARGET Headless)
list(APPEND CMAKE_MESSAGE_INDENT "[${RA_HEADLESS_TARGET}] ")
project(${RA_HEADLESS_TARGET} LANGUAGES CXX VERSION ${RADIUM_VERSION})
# -----------------------------------------------------------------------------------
# lib headless configuration
# -----------------------------------------------------------------------------------
# find required dependencies
find_package(glfw3 3.3 QUIET)
if(NOT glfw3_FOUND)
    # do not configure Radium headless if dependency is not found
    message(WARNING "glfw3 package not found. Headless component is disabled")
    set(RADIUM_MISSING_COMPONENTS ${RADIUM_MISSING_COMPONENTS} ${RA_HEADLESS_TARGET} PARENT_SCOPE)
else()
    # configure the headless component for build and install as component of the Radium package
    message(STATUS "Headless component is enabled")
    # set source files

    include(filelist.cmake)

    add_library(
        ${RA_HEADLESS_TARGET} SHARED ${headless_sources} ${headless_headers} ${headless_inlines}
    )
    add_dependencies(${RA_HEADLESS_TARGET} Core Engine IO)
    target_link_libraries(
        ${RA_HEADLESS_TARGET} PUBLIC Radium::Core Radium::Engine Radium::IO PRIVATE glfw
    )
    target_compile_options(${RA_HEADLESS_TARGET} PUBLIC ${RA_DEFAULT_COMPILE_OPTIONS})
    target_compile_definitions(
        ${RA_HEADLESS_TARGET} PRIVATE ${RA_HEADLESS_TARGET}_EXPORTS GLFW_INCLUDE_NONE
    )
    configure_radium_target(${RA_HEADLESS_TARGET})

    # configure the component "Headless" of packages "Radium"
    set(INSTALLED_HEADERS "${headless_headers};${headless_inlines}")
    configure_radium_library(
        TARGET ${RA_HEADLESS_TARGET} COMPONENT
        PACKAGE_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in FILES "${INSTALLED_HEADERS}"
    )
    set(RADIUM_COMPONENTS ${RADIUM_COMPONENTS} ${RA_HEADLESS_TARGET} PARENT_SCOPE)
endif()

if(RADIUM_ENABLE_PCH)
    target_precompile_headers(${RA_HEADLESS_TARGET} PRIVATE pch.hpp)
endif()

list(REMOVE_AT CMAKE_MESSAGE_INDENT -1)
