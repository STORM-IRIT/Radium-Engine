#-------------------------------------------------------------------------------
# exampleApp executables setup

project(RadiumExampleApps)

# short convenience target
add_custom_target(${PROJECT_NAME})

# Test if this cmake project is included and built in the Radium project build tree
if(${CMAKE_PROJECT_NAME} STREQUAL "radiumproject")
    # Configure in_project build ... These variable are normally set by the find_package(Radium) for
    # client applications
    set(RADIUM_ROOT_DIR "${CMAKE_INSTALL_PREFIX}/")
    set(RADIUM_RESOURCES_DIR "${CMAKE_INSTALL_PREFIX}/Resources/Radium")
    set(RADIUM_PLUGINS_DIR "${CMAKE_INSTALL_PREFIX}/Plugins")
    # This allow to compile the example apps without searching the Radium package when building the
    # demos in the Radium buildtree
    set(IN_RADIUM_BUILD_TREE TRUE)
endif()

foreach(
    APP
    CoreExampleApp
    CustomCameraManipulator
    DrawPrimitivesDemo
    EntityAnimationDemo
    HelloRadium
    KeyEventDemoApp
    PickingDemoApp
    RawShaderMaterial
    SimpleAnimationApp
    SimpleSimulationApp
    TexturedQuad
)
    add_subdirectory(${APP})
    add_dependencies(${PROJECT_NAME} ${APP})
endforeach()

# VolumeDemoApp
get_target_property(HAS_VOLUMES IO RADIUM_IO_HAS_VOLUMES)
if(${HAS_VOLUMES})
    add_subdirectory(VolumeDemoApp)
    add_dependencies(${PROJECT_NAME} VolumeDemoApp)
endif()

# HeadlessDemo headless demo is available only if Radium::headless is available
find_package(Radium COMPONENTS Headless QUIET)
if(Radium_FOUND)
    add_subdirectory(HeadlessDemo)
    add_dependencies(${PROJECT_NAME} HeadlessDemo)
endif()
