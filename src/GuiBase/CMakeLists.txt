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

add_library(
    ${ra_guibase_target} SHARED
    ${guibase_sources}
    ${guibase_headers}
    ${guibase_inlines}
)

configureTargetWithDefaultRaSettings( ${ra_guibase_target} )
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
# set_target_properties(${ra_guibase_target} PROPERTIES PUBLIC_HEADER "${guibase_headers};${guibase_inlines}")

installLibHeaders( "GuiBase" "${guibase_headers}" )
installLibHeaders( "GuiBase" "${guibase_inlines}" )

# Fixme: use a proper ressource manager + install in share ?
message(STATUS "[GuiBase] installing resources from Configs")
installResources(TARGET ${ra_guibase_target} DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Configs)
