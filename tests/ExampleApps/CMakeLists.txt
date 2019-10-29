#-------------------------------------------------------------------------------
# exampleApp executables setup

project(RadiumExampleApps)

# short convenience target from cmake/cleanCpppExtensions.cpp
add_custom_target(test-run)
add_custom_target(${PROJECT_NAME})

# Expectations:
# Sources are in ${CMAKE_CURRENT_SOURCE_DIR}/${APP}
macro( configureExampleApp APP )
    message("[RadiumExampleApps]Configuring ${APP}")

    set(APP_NAME "Radium${APP}")
    set(APP_IDEN "com.radium.${APP}")
    target_include_directories(${APP} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/${APP})

    if(APPLE)
        set_target_properties(${APP} PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME ${APP_NAME})
        set_target_properties(${APP} PROPERTIES MACOSX_BUNDLE_BUNDLE_GUI_IDENTIFIER "${APP_IDEN}")
        set_target_properties(${APP} PROPERTIES MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
        set_target_properties(${APP} PROPERTIES MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")
    endif()

    install(TARGETS ${APP}
        BUNDLE DESTINATION "."
        RUNTIME DESTINATION bin)

    if(APPLE)
        install(CODE "
            include(BundleUtilities)
            fixup_bundle(${CMAKE_INSTALL_PREFIX}/${APP}.app \"\" \"${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}\")
        " COMPONENT Runtime)
    endif()

    # Using macro from cmake/cleanCpppExtensions.cpp included on the top
    # Provides ${APP}-run and ${APP}-dbg
    addRunAndDebugTargets(${APP})

    # short convenience target from cmake/cleanCpppExtensions.cpp
    add_dependencies(test-run "${APP}-run")
    add_dependencies(${PROJECT_NAME} "${APP}")

    # Basic runable tests (will not crash)
    add_test(NAME "${APP}Is_Runable "
        COMMAND $<TARGET_FILE:${APP}>)
endmacro()


add_executable(CoreExampleApp MACOSX_BUNDLE
    CoreExampleApp/CoreExampleApp.cpp)
target_link_libraries (CoreExampleApp PUBLIC Radium::Core)
configureExampleApp( "CoreExampleApp" )

#if( RADIUM_GENERATE_LIB_ENGINE )
#    find_package(Qt5 COMPONENTS Core Widgets OpenGL Xml REQUIRED)
#    find_package(OpenGL REQUIRED)
#    set(CMAKE_AUTOMOC ON)
#    add_executable(EngineExampleApp MACOSX_BUNDLE
#        EngineExampleApp/main.cpp
#        EngineExampleApp/minimalapp.cpp
#        EngineExampleApp/minimalapp.hpp
#        EngineExampleApp/minimalradium.cpp
#        EngineExampleApp/minimalradium.hpp)
#    target_link_libraries (EngineExampleApp PUBLIC
#                           Radium::Core Radium::Engine Radium::GuiBase
#                           Qt5::Core Qt5::Widgets Qt5::OpenGL Qt5::Xml
#                           OpenGL::GL)
#    configureExampleApp( "EngineExampleApp" )
#endif()

