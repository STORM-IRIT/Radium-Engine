#-------------------------------------------------------------------------------
# exampleApp executable setup

add_executable(coreExampleApp MACOSX_BUNDLE
    ${ICON_RESOURCE}
    ${RA_VERSION_CPP}
    CoreExampleApp.cpp)
add_dependencies(coreExampleApp versionFileTouchForRebuild) # We want precise time of build in version
target_include_directories(coreExampleApp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries (coreExampleApp PRIVATE Radium::Core)

if(APPLE)
    set_target_properties(coreExampleApp PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "exampleApp")
    set_target_properties(coreExampleApp PROPERTIES MACOSX_BUNDLE_BUNDLE_GUI_IDENTIFIER "com.exampleApp.exampleApp")
#    set_target_properties(coreExampleApp PROPERTIES MACOSX_BUNDLE_ICON_FILE exampleApp.icns)
#    set_target_properties(coreExampleApp PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${PROJECT_SOURCE_DIR}/packaging/MacOSXBundleInfo.plist.in)
#    set_source_files_properties(${PROJECT_SOURCE_DIR}/packaging/exampleApp.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

install(TARGETS coreExampleApp
    BUNDLE DESTINATION "."
    RUNTIME DESTINATION bin)

# Using macro from cmake/cleanCpppExtensions.cpp included on the top
# Provides coreExampleApp-run and coreExampleApp-dbg
addRunAndDebugTargets(coreExampleApp)

# short convenience target from cmake/cleanCpppExtensions.cpp
add_custom_target(test-run DEPENDS coreExampleApp-run)


# Basic runable tests (will not crash)
add_test(NAME "Is_Runable "
    COMMAND $<TARGET_FILE:coreExampleApp>)

add_test(NAME "Is_Runable-v"
    COMMAND $<TARGET_FILE:coreExampleApp> -v)

add_test(NAME "Is_Runable-h"
    COMMAND $<TARGET_FILE:coreExampleApp> -h)
