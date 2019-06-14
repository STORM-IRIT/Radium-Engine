# Find packages
find_package(OpenGL     REQUIRED)
find_package(Qt5 COMPONENTS Core Widgets OpenGL Xml REQUIRED)

if(Qt5Core_VERSION VERSION_LESS 5.5 OR
   Qt5Widgets_VERSION VERSION_LESS 5.5 OR
   Qt5OpenGL_VERSION VERSION_LESS 5.5)
    message(FATAL_ERROR "Qt5 or superior required (found ${Qt5Core_VERSION}).")
else ()
    message(STATUS "QT ${Qt5Core_VERSION} found." )
endif()

# Build GuiBase
set(guibase_target radiumGuiBase)

# Qt5
set(Qt5_LIBRARIES Qt5::Core Qt5::Widgets Qt5::OpenGL Qt5::Xml)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Fixme : Really usefull ?
add_definitions(${Qt5Core_DEFINITIONS})
add_definitions(${Qt5Widgets_DEFINITIONS})
add_definitions(${Qt5OpenGL_DEFINITIONS})
add_definitions(${Qt5Xml_DEFINITIONS})

file(GLOB_RECURSE guibase_sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE guibase_headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB_RECURSE guibase_inlines ${CMAKE_CURRENT_SOURCE_DIR}/*.inl)

set (guibase_libs
    radiumCore
    radiumEngine
    radiumIO
    radiumPluginBase
    ${Qt5_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${GLBINDING_LIBRARIES}
    ${GLOBJECTS_LIBRARIES}
)

add_library(
    ${guibase_target} SHARED
    ${guibase_sources}
    ${guibase_headers}
    ${guibase_inlines})

target_include_directories(${guibase_target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_dependencies( ${guibase_target} radiumCore radiumEngine radiumIO radiumPluginBase glbinding_lib globjects_lib stb_lib)
target_compile_definitions(${guibase_target} PRIVATE "-DRA_GUIBASE_EXPORTS")

target_link_libraries(
    ${guibase_target}
    PUBLIC ${guibase_libs}
    )

#radium_cotire(${guibase_target})

set_property( TARGET ${guibase_target} PROPERTY POSITION_INDEPENDENT_CODE ON )
