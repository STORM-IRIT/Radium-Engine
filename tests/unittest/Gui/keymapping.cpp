#include <Core/Resources/Resources.hpp>
#include <Core/Utils/StdFilesystem.hpp>
#include <Gui/Utils/KeyMappingManager.hpp>

#include <catch2/catch.hpp>

#include <QtGlobal>

using namespace Ra;
using namespace Ra::Gui;
using Idx = KeyMappingManager::KeyMappingAction;
using Ctx = KeyMappingManager::Context;

TEST_CASE( "Gui/Utils/KeyMappingManager", "[Gui][Gui/Utils][KeyMappingManager]" ) {
    QSettings settings( "RadiumUnitTests", "KeyMappingManager" );

    settings.clear();
    KeyMappingManager::createInstance();
    auto mgr = Gui::KeyMappingManager::getInstance();
    auto optionalPath {Core::Resources::getRadiumResourcesPath()};
    auto resourcesRootDir {optionalPath.value_or( "[[Default resrouces path not found]]" )};
    ///\todo how to check here ?
    auto defaultConfigFile = resourcesRootDir +
#ifndef OS_MACOS
                             std::string( "Configs/default.xml" )
#else
                             std::string( "Configs/macos.xml" )
#endif
        ;

    SECTION( "key mapping file load" ) {
        REQUIRE( defaultConfigFile == mgr->getLoadedFilename() );
        mgr->loadConfiguration( "dummy" );
        REQUIRE( defaultConfigFile == mgr->getLoadedFilename() );
        std::cout << std::filesystem::current_path() << "\n";
        mgr->loadConfiguration( "data/keymapping-valid.xml" );
        REQUIRE( "data/keymapping-valid.xml" == mgr->getLoadedFilename() );
        // invalid xml should not be loaded, and switch to default
        mgr->loadConfiguration( "data/keymapping-invalid.xml" );
        REQUIRE( defaultConfigFile == mgr->getLoadedFilename() );
        // while config error are loaded, with error message
        mgr->loadConfiguration( "data/keymapping-double-actions.xml" );
        REQUIRE( "data/keymapping-double-actions.xml" == mgr->getLoadedFilename() );
        // bad tag load defaults
        mgr->loadConfiguration( "data/keymapping-bad-tag.xml" );
        REQUIRE( defaultConfigFile == mgr->getLoadedFilename() );
        // bad main tag loads with a warning
        mgr->loadConfiguration( "data/keymapping-bad-main.xml" );
        REQUIRE( "data/keymapping-bad-main.xml" == mgr->getLoadedFilename() );
    }

    SECTION( "getAction" ) {
        mgr->loadConfiguration( "data/keymapping-valid.xml" );
        // check context
        auto cameraContext {mgr->getContext( "CameraContext" )};
        REQUIRE( cameraContext.isValid() );
        REQUIRE( mgr->getContextName( cameraContext ) == "CameraContext" );

        auto viewerContext {mgr->getContext( "ViewerContext" )};
        REQUIRE( viewerContext.isValid() );
        REQUIRE( viewerContext != cameraContext );

        auto invalidContext {mgr->getContext( "InvalidContext" )};
        REQUIRE( invalidContext.isInvalid() );
        // invalidContext index returns "Invalid" context name
        REQUIRE( mgr->getContextName( Ctx {} ) == "Invalid" );
        REQUIRE( mgr->getContextName( Ctx {42} ) == "Invalid" );

        // test on action
        auto validAction {mgr->getAction( cameraContext, Qt::LeftButton, Qt::NoModifier, -1 )};
        REQUIRE( validAction.isValid() );
        REQUIRE( validAction == mgr->getActionIndex( cameraContext, "TRACKBALLCAMERA_ROTATE" ) );
        REQUIRE( mgr->getActionIndex( Ctx {}, "TRACKBALLCAMERA_ROTATE" ).isInvalid() );
        REQUIRE( mgr->getActionIndex( Ctx {42}, "TRACKBALLCAMERA_ROTATE" ).isInvalid() );
        REQUIRE( mgr->getActionName( cameraContext, validAction ) == "TRACKBALLCAMERA_ROTATE" );

        // invalid action index returns "Invalid" action name
        REQUIRE( mgr->getActionName( cameraContext, Idx {} ) == "Invalid" );

        // modifiers as key are ignored
        validAction = mgr->getAction( cameraContext, Qt::LeftButton, Qt::ShiftModifier, -1 );
        auto action2 {
            mgr->getAction( cameraContext, Qt::LeftButton, Qt::ShiftModifier, Qt::Key_Shift )};
        REQUIRE( validAction == action2 );

        // tests some invalid actions
        auto invalidAction {mgr->getAction( cameraContext, Qt::LeftButton, Qt::AltModifier, -1 )};
        REQUIRE( invalidAction.isInvalid() );
        REQUIRE( invalidAction != mgr->getActionIndex( cameraContext, "TRACKBALLCAMERA_ROTATE" ) );
        REQUIRE( mgr->getAction( Idx {}, Qt::LeftButton, Qt::AltModifier, -1 ).isInvalid() );
        REQUIRE( mgr->getAction( Idx {}, Qt::LeftButton, Qt::AltModifier, 1 ).isInvalid() );
        REQUIRE( mgr->getAction( Idx {}, Qt::LeftButton, Qt::NoModifier, -1 ).isInvalid() );
        REQUIRE( mgr->getAction( Idx {}, Qt::RightButton, Qt::AltModifier, -1 ).isInvalid() );

        // with key and modifiers
        validAction = mgr->getAction( viewerContext, Qt::RightButton, Qt::NoModifier, Qt::Key_V );
        REQUIRE( validAction.isValid() );
        REQUIRE( validAction == mgr->getActionIndex( viewerContext, "VIEWER_PICKING_VERTEX" ) );

        // action index
        REQUIRE( mgr->getActionIndex( viewerContext, "UnkownAction" ).isInvalid() );
        validAction = mgr->getAction( viewerContext, Qt::NoButton, Qt::ControlModifier, Qt::Key_W );
        REQUIRE( validAction.isValid() );
        REQUIRE( validAction == mgr->getActionIndex( viewerContext, "VIEWER_TOGGLE_WIREFRAME" ) );

        // action are context dependent
        invalidAction = mgr->getAction( viewerContext, Qt::LeftButton, Qt::NoModifier, -1 );
        REQUIRE( invalidAction.isInvalid() );
        REQUIRE( mgr->getActionIndex( cameraContext, "VIEWER_TOGGLE_WIREFRAME" ).isInvalid() );
    }
}
