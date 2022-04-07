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

# CoreExampleApp
add_subdirectory(CoreExampleApp)
add_dependencies(${PROJECT_NAME} CoreExampleApp)

# CustomCameraManipulator
add_subdirectory(CustomCameraManipulator)
add_dependencies(${PROJECT_NAME} CustomCameraManipulator)

# DrawPrimitivesDemo
add_subdirectory(DrawPrimitivesDemo)
add_dependencies(${PROJECT_NAME} DrawPrimitivesDemo)

# EntityAnimationDemo
add_subdirectory(EntityAnimationDemo)
add_dependencies(${PROJECT_NAME} EntityAnimationDemo)

# HeadlessDemo
add_subdirectory(HeadlessDemo)
add_dependencies(${PROJECT_NAME} HeadlessDemo)

# HelloRadium
add_subdirectory(HelloRadium)
add_dependencies(${PROJECT_NAME} HelloRadium)

# KeyEventDemoApp
add_subdirectory(KeyEventDemoApp)
add_dependencies(${PROJECT_NAME} KeyEventDemoApp)

# PickingDemoApp
add_subdirectory(PickingDemoApp)
add_dependencies(${PROJECT_NAME} PickingDemoApp)

# RawShaderMaterial
add_subdirectory(RawShaderMaterial)
add_dependencies(${PROJECT_NAME} RawShaderMaterial)

# SimpleAnimationApp
add_subdirectory(SimpleAnimationApp)
add_dependencies(${PROJECT_NAME} SimpleAnimationApp)

# SimpleSimulationApp
add_subdirectory(SimpleSimulationApp)
add_dependencies(${PROJECT_NAME} SimpleSimulationApp)

# VolumeDemoApp
get_target_property(HAS_VOLUMES IO RADIUM_IO_HAS_VOLUMES)
if(${HAS_VOLUMES})
    add_subdirectory(VolumeDemoApp)
    add_dependencies(${PROJECT_NAME} VolumeDemoApp)
endif()
