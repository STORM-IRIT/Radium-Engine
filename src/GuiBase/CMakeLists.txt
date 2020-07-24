set(ra_guibase_target GuiBase)

project( ${ra_guibase_target}
         LANGUAGES CXX
         VERSION ${RADIUM_VERSION})

include( filelistGuiBase )

# Find packages
find_package(Qt5 COMPONENTS Core Widgets OpenGL Xml REQUIRED)
find_package(OpenGL REQUIRED)

if(Qt5Core_VERSION VERSION_LESS 5.5)
    message(FATAL_ERROR "Qt5 or superior required (found ${Qt5Core_VERSION}).")
else ()
    message(STATUS "QT ${Qt5Core_VERSION} found." )
endif()

# Qt5
set(Qt5_LIBRARIES Qt5::Core Qt5::Widgets Qt5::OpenGL Qt5::Xml)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR} # Moc
    )
qt5_wrap_ui(guibase_uis ${guibase_uis})

add_library(
    ${ra_guibase_target} SHARED
    ${guibase_sources}
    ${guibase_headers}
    ${guibase_inlines}
    ${guibase_uis}
    ${guibase_resources}
)

add_dependencies(${ra_guibase_target} Core Engine PluginBase IO)
target_link_libraries(${ra_guibase_target} PUBLIC Core Engine PluginBase IO)
target_link_libraries (${ra_guibase_target} PRIVATE ${Qt5_LIBRARIES} OpenGL::GL)

if( CMAKE_BUILD_TYPE MATCHES Debug )
    target_compile_definitions(${ra_guibase_target} PUBLIC GUIBASE_IS_COMPILED_WITH_DEBUG_INFO)
endif()

# Ask RadiumIO for supported loaders
get_target_property(USE_ASSIMP IO IO_ASSIMP)
if (${USE_ASSIMP})
    target_compile_definitions(${ra_guibase_target} PRIVATE "-DIO_USE_ASSIMP")
endif ()
get_target_property(USE_TINYPLY IO IO_TINYPLY)
if (${USE_TINYPLY})
    target_compile_definitions(${ra_guibase_target} PRIVATE "-DIO_USE_TINYPLY")
endif ()

target_compile_definitions(${ra_guibase_target} PRIVATE "-DRA_GUIBASE_EXPORTS")

message(STATUS "Configuring library ${ra_guibase_target} with standard settings")
configureTargetWithDefaultRaSettings(${ra_guibase_target})
configure_radium_library(
    TARGET ${ra_guibase_target}
    FILES "${guibase_headers};${guibase_inlines}"
)
message(STATUS "[GuiBase] installing resources from Configs")
installTargetResources(
    TARGET ${ra_guibase_target}
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Configs
)
