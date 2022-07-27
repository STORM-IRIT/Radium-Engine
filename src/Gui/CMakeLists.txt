set(RA_GUI_TARGET Gui)
list(APPEND CMAKE_MESSAGE_INDENT "[${RA_GUI_TARGET}] ")

project(${RA_GUI_TARGET} LANGUAGES CXX VERSION ${RADIUM_VERSION})

include(filelist.cmake)

# Qt utility functions
include(QtFunctions)

# Find packages
find_qt_package(COMPONENTS Core Widgets OpenGL Xml REQUIRED)
set(QT_DEFAULT_MAJOR_VERSION ${QT_DEFAULT_MAJOR_VERSION} PARENT_SCOPE)

find_package(OpenGL REQUIRED)

# Qt
set(QT_LIBRARIES Qt::Core Qt::Widgets Qt::OpenGL Qt::Xml)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${CMAKE_CURRENT_BINARY_DIR} # Moc
)

qt_wrap_ui(GUI_UIS ${GUI_UIS})

add_library(
    ${RA_GUI_TARGET} SHARED ${GUI_SOURCES} ${GUI_HEADERS} ${GUI_INLINES} ${GUI_UIS}
                            ${GUI_RESOURCES}
)

add_dependencies(${RA_GUI_TARGET} Core Engine PluginBase IO)
target_link_libraries(${RA_GUI_TARGET} PUBLIC Core Engine PluginBase IO)
target_link_libraries(${RA_GUI_TARGET} PRIVATE ${QT_LIBRARIES} OpenGL::GL)

target_compile_options(${RA_GUI_TARGET} PUBLIC ${RA_DEFAULT_COMPILE_OPTIONS})

if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(${RA_GUI_TARGET} PUBLIC GUI_IS_COMPILED_WITH_DEBUG_INFO)
endif()

# Ask RadiumIO for supported loaders
get_target_property(USE_ASSIMP IO IO_HAS_ASSIMP)
if(${USE_ASSIMP})
    target_compile_definitions(${RA_GUI_TARGET} PRIVATE "-DIO_HAS_ASSIMP")
endif()
get_target_property(USE_TINYPLY IO IO_HAS_TINYPLY)
if(${USE_TINYPLY})
    target_compile_definitions(${RA_GUI_TARGET} PRIVATE "-DIO_HAS_TINYPLY")
endif()
get_target_property(HAS_VOLUMES IO IO_HAS_VOLUMES)
if(${HAS_VOLUMES})
    target_compile_definitions(${RA_GUI_TARGET} PRIVATE "-DIO_HAS_VOLUMES")
endif()

target_compile_definitions(${RA_GUI_TARGET} PRIVATE "-DRA_GUI_EXPORTS")

message(STATUS "Configuring library ${RA_GUI_TARGET} with standard settings")
configure_radium_target(${RA_GUI_TARGET})
configure_radium_library(
    TARGET ${RA_GUI_TARGET} COMPONENT PACKAGE_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    FILES "${GUI_HEADERS};${GUI_INLINES}"
)
set(RADIUM_COMPONENTS ${RADIUM_COMPONENTS} ${RA_GUI_TARGET} PARENT_SCOPE)

message(STATUS "installing resources from Configs")
install_target_resources(
    TARGET ${RA_GUI_TARGET} RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Configs
    RESOURCES_INSTALL_DIR "Radium"
)

if(RADIUM_ENABLE_PCH)
    target_precompile_headers(${RA_GUI_TARGET} PRIVATE pch.hpp)
endif()

list(REMOVE_AT CMAKE_MESSAGE_INDENT -1)
