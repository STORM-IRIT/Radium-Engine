#include "FancyMeshPlugin.hpp"

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include <Core/Log/Log.hpp>
#include <Engine/RadiumEngine.hpp>

#include "FancyMeshSystem.hpp"

namespace FancyMeshPlugin
{

    FancyMeshPluginC::~FancyMeshPluginC()
    {
    }

    void FancyMeshPluginC::registerPlugin( Ra::Engine::RadiumEngine* engine )
    {
        FancyMeshSystem* system = new FancyMeshSystem;
        engine->registerSystem( "FancyMeshSystem", system );
    }

    bool FancyMeshPluginC::doAddWidget( QString &name )
    {
        name = "Fancy Mesh";
        return true;
    }

    QWidget* FancyMeshPluginC::getWidget()
    {
        QWidget* widget = new QWidget;

        QVBoxLayout* layout = new QVBoxLayout( widget );
        QLabel* label = new QLabel( "I am a FancyMesh label" );
        QPushButton* button = new QPushButton( "And I am a FancyMesh button" );

        layout->addWidget( label );
        layout->addWidget( button );

        return widget;
    }

    bool FancyMeshPluginC::doAddMenu()
    {
        return true;
    }

    QMenu* FancyMeshPluginC::getMenu()
    {
        QMenu* menu = new QMenu( "FancyMesh" );

        menu->addAction( "Hello there" );
        menu->addAction( "And there" );

        return menu;
    }
}
