#-------------------------------------------------------------------------------
# exampleApp executables setup

project(RadiumExamples)

list(APPEND CMAKE_MESSAGE_INDENT "[Examples] ")

# short convenience target
add_custom_target(${PROJECT_NAME})

foreach(
    APP
    CoreExample
    CustomCameraManipulator
    DrawPrimitives
    EntityAnimation
    EnvMap
    HelloRadium
    KeyEvent
    Picking
    RawShaderMaterial
    SimpleAnimation
    SimpleSimulation
    TexturedQuad
)
    add_subdirectory(${APP})
    add_dependencies(${PROJECT_NAME} ${APP})
endforeach()

# VolumeDemoApp
get_target_property(HAS_VOLUMES IO RADIUM_IO_HAS_VOLUMES)
if(${HAS_VOLUMES})
    add_subdirectory(Volume)
    add_dependencies(${PROJECT_NAME} Volume)
endif()

# HeadlessDemo headless demo is available only if Radium::headless is available
find_package(Radium COMPONENTS Headless QUIET)
if(Radium_FOUND)
    add_subdirectory(HeadlessExample)
    add_dependencies(${PROJECT_NAME} HeadlessExample)
endif()

# Define specific installation procedure for this example to prevent mixed installation with Radium
# bundle
set(OLD_CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
set(CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/PluginsExample")
add_subdirectory(PluginsWithLib EXCLUDE_FROM_ALL)
set(CMAKE_INSTALL_PREFIX "${OLD_CMAKE_INSTALL_PREFIX}")
add_dependencies(${PROJECT_NAME} PluginsWithLib)
install(
    CODE "execute_process(COMMAND ${CMAKE_COMMAND} --build \"${CMAKE_CURRENT_BINARY_DIR}/PluginsWithLib\" --target install)"
)

list(REMOVE_AT CMAKE_MESSAGE_INDENT -1)
