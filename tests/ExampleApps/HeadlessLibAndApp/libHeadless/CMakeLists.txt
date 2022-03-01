cmake_minimum_required(VERSION 3.13)
if(APPLE)
    cmake_policy(SET CMP0042 NEW)
endif(APPLE)

project(RadiumHeadless VERSION 1.0.2)

# Defines the RadiumHeadless version in the parent scope so that one can use this lib doing
# add_subdirectory(...) and access RadiumHeadless_VERSION
set(${PROJECT_NAME}_VERSION ${PROJECT_VERSION} PARENT_SCOPE)
set(${PROJECT_NAME}_VERSION_MAJOR ${PROJECT_VERSION_MAJOR} PARENT_SCOPE)
set(${PROJECT_NAME}_VERSION_MINOR ${PROJECT_VERSION_MINOR} PARENT_SCOPE)
set(${PROJECT_NAME}_VERSION_PATCH ${PROJECT_VERSION_PATCH} PARENT_SCOPE)

# ------------------------------------------------------------------------------
# set wanted application defaults for cmake settings
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
# Set default install location to installed-<Compiler_ID> folder in build dir we do not want to
# install to /usr by default
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX
        "${CMAKE_CURRENT_BINARY_DIR}/installed-${CMAKE_CXX_COMPILER_ID}-${CMAKE_BUILD_TYPE}"
        CACHE PATH "Install path prefix, prepended onto install directories." FORCE
    )
    message("Set install prefix to ${CMAKE_INSTALL_PREFIX}")
endif()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

# -----------------------------------------------------------------------------------
# lib headless configuration
# -----------------------------------------------------------------------------------
set(libName Headless)
set(namespaceName Radium)

# Radium components : IO is not mandatory and should be removed once verified
if(NOT IN_RADIUM_BUILD_TREE)
    find_package(Radium REQUIRED Core Engine IO)
    message(STATUS "Found Radium version ${Radium_VERSION}")
    set(DEFAULT_COMPILE_OPTIONS)
endif()

find_package(glfw3 3.2 REQUIRED)

set(CLI11_headers
    RadiumHeadless/CLI/App.hpp
    RadiumHeadless/CLI/CLI.hpp
    RadiumHeadless/CLI/Config.hpp
    RadiumHeadless/CLI/ConfigFwd.hpp
    RadiumHeadless/CLI/Error.hpp
    RadiumHeadless/CLI/Formatter.hpp
    RadiumHeadless/CLI/FormatterFwd.hpp
    RadiumHeadless/CLI/Macros.hpp
    RadiumHeadless/CLI/Option.hpp
    RadiumHeadless/CLI/Split.hpp
    RadiumHeadless/CLI/StringTools.hpp
    RadiumHeadless/CLI/Timer.hpp
    RadiumHeadless/CLI/TypeTools.hpp
    RadiumHeadless/CLI/Validators.hpp
    RadiumHeadless/CLI/Version.hpp
)

set(${libName}_sources RadiumHeadless/CLIViewer.cpp RadiumHeadless/OpenGLContext/OpenGLContext.cpp)

set(${libName}_headers
    ${CLI11_headers} RadiumHeadless/CLIBaseApplication.hpp RadiumHeadless/CLIViewer.hpp
    RadiumHeadless/Headless.hpp RadiumHeadless/OpenGLContext/OpenGLContext.hpp
)

add_library(${libName} SHARED ${${libName}_sources} ${${libName}_headers})

target_include_directories(${libName} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/RadiumHeadless)

target_link_libraries(${libName} PUBLIC Radium::Core Radium::Engine Radium::IO PRIVATE glfw)

target_compile_definitions(${libName} PRIVATE ${libName}_EXPORTS GLFW_INCLUDE_NONE)

target_compile_options(${libName} PRIVATE PUBLIC ${DEFAULT_COMPILE_OPTIONS} INTERFACE)

# configure the component "Headless" of pacakges "RadiumHeadless"
configure_radium_library(
    TARGET ${libName}
    FILES "${${libName}_headers}"
    TARGET_DIR .
    NAMESPACE ${namespaceName}
    PACKAGE_DIR ${CMAKE_INSTALL_PREFIX}/lib/cmake/${namespaceName}
)

# configure the package "RadiumHeadless"
configure_radium_package(
    NAME "Radium${libName}" PACKAGE_DIR ${CMAKE_INSTALL_PREFIX}/lib/cmake/${namespaceName}
    PACKAGE_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in PACKAGE_VERSION ${PROJECT_VERSION}
)
