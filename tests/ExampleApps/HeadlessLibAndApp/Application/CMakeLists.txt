cmake_minimum_required(VERSION 3.13)
if(APPLE)
    cmake_policy(SET CMP0042 NEW)
endif(APPLE)

project(HeadlessDemo VERSION 1.0.2)

if(NOT CMAKE_BUILD_TYPE)
    message_info("No CMAKE_BUILD_TYPE, using default CMAKE_BUILD_TYPE=Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()

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

if(NOT IN_LIBHEADLESS_BUILD_TREE)
    # package RadiumHeadless will import its Radium dependencies
    find_package(RadiumHeadless REQUIRED)
elseif(NOT IN_RADIUM_BUILD_TREE)
    # need this to import Radium dependencies if not using the package RadiumHeadless but the
    # subdirectory
    find_package(Radium REQUIRED Core Engine IO)
endif()

option(USE_RADIUM_IO_ASSIMPLOADER "Use assimp data loader from Radium IO" ON)
message(
    STATUS
        "Configuring demo application ${PROJECT_NAME} with RadiumHeadless ${RadiumHeadless_VERSION} and Radium ${Radium_VERSION}"
)

set(app_sources main.cpp)

add_executable(${PROJECT_NAME} ${app_sources})

target_link_libraries(${PROJECT_NAME} PUBLIC Radium::Headless)

if(USE_RADIUM_IO_ASSIMPLOADER)
    if(IN_RADIUM_BUILD_TREE)
        set(USE_ASSIMP ${RADIUM_IO_ASSIMP})
    else()
        get_target_property(USE_RADIUM_ASSIMP Radium::IO RADIUM_IO_USE_ASSIMP)
    endif()
    if(${USE_RADIUM_ASSIMP})
        message(STATUS "[${PROJECT_NAME}] Enabling assimp loader from Radium::IO")
        target_compile_definitions(${PROJECT_NAME} PRIVATE ADD_ASSIMP_LOADER)
    else()
        message(STATUS "[${PROJECT_NAME}] Assimp loader from Radium::IO not detected")
    endif()
endif()

target_compile_options(${PROJECT_NAME} PRIVATE PUBLIC ${DEFAULT_COMPILE_OPTIONS} INTERFACE)

configure_radium_app(NAME ${PROJECT_NAME} PREFIX Demos RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Assets)
