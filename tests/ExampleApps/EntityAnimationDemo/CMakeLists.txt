cmake_minimum_required(VERSION 3.13)
if(APPLE)
    cmake_policy(SET CMP0042 NEW)
endif(APPLE)

project(EntityAnimationDemo VERSION 1.0.0)

set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/installed-${CMAKE_CXX_COMPILER_ID}"
        CACHE PATH "Install path prefix, prepended onto install directories." FORCE
    )
    message("Set install prefix to ${CMAKE_INSTALL_PREFIX} for project ${PROJECT_NAME}.")
    set(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT False)
endif()

# ------------------------------------------------------------------------------
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
set(Qt5_LIBRARIES Qt5::Core Qt5::Widgets)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# ------------------------------------------------------------------------------
if(NOT IN_RADIUM_BUILD_TREE)
    find_package(Radium REQUIRED Core Engine Gui IO PluginBase)
endif()
# ------------------------------------------------------------------------------

set(app_sources main.cpp)

add_executable(${PROJECT_NAME} ${app_sources})

target_link_libraries(
    ${PROJECT_NAME} PUBLIC Radium::Core Radium::Engine Radium::IO Radium::Gui ${Qt5_LIBRARIES}
)

target_compile_options(${PROJECT_NAME} PRIVATE PUBLIC ${DEFAULT_COMPILE_OPTIONS} INTERFACE)

configure_radium_app(NAME ${PROJECT_NAME})
