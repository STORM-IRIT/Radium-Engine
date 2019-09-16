set(ra_guibase_target RadiumGuiBase)

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
add_library(Radium::GuiBase ALIAS ${ra_guibase_target})

configureTargetWithDefaultRaSettings( ${ra_guibase_target} )
add_dependencies( ${ra_guibase_target} Radium::Core Radium::Engine Radium::PluginBase )
target_link_libraries (${ra_guibase_target} PUBLIC Radium::Core Radium::Engine Radium::PluginBase ${Qt5_LIBRARIES} OpenGL::GL)

target_compile_definitions(${ra_guibase_target} PRIVATE "-DRA_GUIBASE_EXPORTS")
# set_target_properties(${ra_guibase_target} PROPERTIES PUBLIC_HEADER "${guibase_headers};${guibase_inlines}")

install(TARGETS ${ra_guibase_target}
    EXPORT ${ra_guibase_target}Targets
    CONFIGURATIONS ${CMAKE_BUILD_TYPE}
    RUNTIME  DESTINATION bin
    OBJECTS  DESTINATION bin
    LIBRARY  DESTINATION lib
    ARCHIVE  DESTINATION lib
    INCLUDES DESTINATION include
    )

installLibHeaders( "GuiBase" "${guibase_headers}" )
installLibHeaders( "GuiBase" "${guibase_inlines}" )

# Fixme: use a proper ressource manager + install in share, not bin
install( DIRECTORY Configs DESTINATION bin )
