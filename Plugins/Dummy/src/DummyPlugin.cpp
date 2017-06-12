#include "DummyPlugin.hpp"

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>

#include <Core/Log/Log.hpp>
#include <Engine/RadiumEngine.hpp>

#include "DummySystem.hpp"

namespace DummyPlugin
{

    DummyPlugin::~DummyPlugin()
    {
    }

    void DummyPlugin::registerPlugin(const Ra::PluginContext &context )
    {
        DummySystem* system = new DummySystem;
        context->registerSystem( "DummySystem", system );
    }

    bool DummyPlugin::doAddWidget( QString &name )
    {
        name = "Dummy Mesh";
        return true;
    }

    QWidget* DummyPlugin::getWidget()
    {
        QWidget* widget = new QWidget;

        m_label = new QLabel( "I am a dummy label", widget );
        QPushButton* button = new QPushButton( "And I am a dummy button", widget );

        QVBoxLayout* layout = new QVBoxLayout( widget );

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
        QMenu* menu = new QMenu( "Dummy" );

        QAction* a1 = menu->addAction( "Hello there" );
        QAction* a2 = menu->addAction( "And there" );

        CORE_UNUSED( a1 );
        CORE_UNUSED( a2 );

        return menu;
    }

    void DummyPlugin::sayHello()
    {
        m_label->setText( "I have been clicked at least once !" );
        QMessageBox::warning( nullptr, "Dummy Plugin", "Hello, Radium World !");
    }
}
