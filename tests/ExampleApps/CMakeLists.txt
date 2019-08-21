#-------------------------------------------------------------------------------
# exampleApp executable setup

project(coreExampleApp)

add_executable(${PROJECT_NAME} MACOSX_BUNDLE
    ${ICON_RESOURCE}
    ${RA_VERSION_CPP}
    CoreExampleApp.cpp)
add_dependencies(${PROJECT_NAME} versionFileTouchForRebuild) # We want precise time of build in version
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries (${PROJECT_NAME} PUBLIC Radium::Core)

if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "RadiumCoreExampleApp")
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_BUNDLE_GUI_IDENTIFIER "com.radium.exampleApp")
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")
endif()

install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION "."
    RUNTIME DESTINATION bin)

if(APPLE)
    include( BundleUtilities )
    install(CODE "
        include(BundleUtilities)
        fixup_bundle(${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app \"\" \"${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}\")
    " COMPONENT Runtime)
endif()

# Using macro from cmake/cleanCpppExtensions.cpp included on the top
# Provides ${PROJECT_NAME}-run and ${PROJECT_NAME}-dbg
addRunAndDebugTargets(${PROJECT_NAME})

# short convenience target from cmake/cleanCpppExtensions.cpp
add_custom_target(test-run DEPENDS ${PROJECT_NAME}-run)


# Basic runable tests (will not crash)
add_test(NAME "Is_Runable "
    COMMAND $<TARGET_FILE:${PROJECT_NAME}>)

add_test(NAME "Is_Runable-v"
    COMMAND $<TARGET_FILE:${PROJECT_NAME}> -v)

add_test(NAME "Is_Runable-h"
    COMMAND $<TARGET_FILE:${PROJECT_NAME}> -h)


