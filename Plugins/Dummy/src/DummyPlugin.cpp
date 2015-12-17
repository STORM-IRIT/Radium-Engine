#include "DummyPlugin.hpp"

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include <Core/Log/Log.hpp>
#include <Engine/RadiumEngine.hpp>

#include "DummySystem.hpp"

namespace DummyPlugin
{

    DummyPlugin::~DummyPlugin()
    {
    }

    void DummyPlugin::registerPlugin( Ra::Engine::RadiumEngine* engine )
    {
        DummySystem* system = new DummySystem;
        engine->registerSystem( "DummySystem", system );
    }

    bool DummyPlugin::doAddWidget( QString &name )
    {
        name = "Dummy Mesh";
        return true;
    }

    QWidget* DummyPlugin::getWidget()
    {
        QWidget* widget = new QWidget;

        m_label = new QLabel( "I am a dummy label" );

        QVBoxLayout* layout = new QVBoxLayout( widget );
        QPushButton* button = new QPushButton( "And I am a dummy button" );

        layout->addWidget( m_label );
        layout->addWidget( button );

        connect( button, &QPushButton::pressed, this, &DummyPlugin::sayHello );

        return widget;
    }

    bool DummyPlugin::doAddMenu()
    {
        return true;
    }

    QMenu* DummyPlugin::getMenu()
    {
        QMenu* menu = new QMenu( "FancyMesh" );

        menu->addAction( "Hello there" );
        menu->addAction( "And there" );

        return menu;
    }

    void DummyPlugin::sayHello()
    {
        m_label->setText( "I have been clicked at least once !" );
    }
}
