set(RA_PLUGINBASE_TARGET PluginBase)
list(APPEND CMAKE_MESSAGE_INDENT "[${RA_PLUGINBASE_TARGET}] ")

project(${RA_PLUGINBASE_TARGET} LANGUAGES CXX VERSION ${RADIUM_VERSION})

include(filelist.cmake)

# Qt utility functions
include(QtFunctions)

# Find packages
find_qt_package(COMPONENTS Core REQUIRED)

# Qt
set(QT_LIBRARIES Qt::Core)
set(CMAKE_AUTOMOC ON)

add_library(
    ${RA_PLUGINBASE_TARGET} SHARED ${pluginbase_sources} ${pluginbase_headers}
                                   ${pluginbase_inlines} ${pluginbase_json}
)

add_dependencies(${RA_PLUGINBASE_TARGET} Core Engine)
target_link_libraries(${RA_PLUGINBASE_TARGET} PUBLIC Core Engine)
target_link_libraries(${RA_PLUGINBASE_TARGET} PRIVATE Qt::Core)

target_compile_options(${RA_PLUGINBASE_TARGET} PUBLIC ${RA_DEFAULT_COMPILE_OPTIONS})

target_compile_definitions(${RA_PLUGINBASE_TARGET} PRIVATE "-DRA_PLUGINBASE_EXPORTS")

message(STATUS "Configuring library ${RA_PLUGINBASE_TARGET} with standard settings")
configure_radium_target(${RA_PLUGINBASE_TARGET})
configure_radium_library(
    TARGET ${RA_PLUGINBASE_TARGET} COMPONENT
    PACKAGE_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    FILES "${pluginbase_headers};${pluginbase_inlines}"
)
set(RADIUM_COMPONENTS ${RADIUM_COMPONENTS} ${RA_PLUGINBASE_TARGET} PARENT_SCOPE)

# install json configuration files
install(FILES ${pluginbase_json} DESTINATION ${CONFIG_PACKAGE_LOCATION}/${RA_PLUGINBASE_TARGET})
file(COPY ${pluginbase_json} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

list(REMOVE_AT CMAKE_MESSAGE_INDENT -1)
