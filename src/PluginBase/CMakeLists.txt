# Find packages
find_package(Qt5 COMPONENTS Core REQUIRED)

if(Qt5Core_VERSION VERSION_LESS 5.5)
    message(FATAL_ERROR "Qt5 or superior required (found ${Qt5Core_VERSION}).")
else ()
    message(STATUS "QT ${Qt5Core_VERSION} found." )
endif()

# Build PluginBase
set(pluginbase_target radiumPluginBase)

# Qt5
set(Qt5_LIBRARIES Qt5::Core)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


file(GLOB_RECURSE pluginbase_sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE pluginbase_headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB_RECURSE pluginbase_inlines ${CMAKE_CURRENT_SOURCE_DIR}/*.inl)

set (pluginbase_libs
    radiumCore
    radiumEngine
    ${Qt5_LIBRARIES}
)

add_library(
    ${pluginbase_target} SHARED
    ${pluginbase_sources}
    ${pluginbase_headers}
    ${pluginbase_inlines})

target_include_directories(${pluginbase_target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_dependencies( ${pluginbase_target} radiumCore radiumEngine )
target_compile_definitions(${pluginbase_target} PRIVATE "-DRA_PLUGINBASE_EXPORTS")

target_link_libraries(
    ${pluginbase_target}
    PUBLIC ${pluginbase_libs}
    )

#radium_cotire(${pluginbase_target})

set_property( TARGET ${pluginbase_target} PROPERTY POSITION_INDEPENDENT_CODE ON )
