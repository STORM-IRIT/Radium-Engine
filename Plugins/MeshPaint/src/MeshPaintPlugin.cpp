#include <MeshPaintPlugin.hpp>

#include <QAction>
#include <QIcon>
#include <QToolBar>

#include <Engine/Entity/Entity.hpp>
#include <Engine/Managers/EntityManager/EntityManager.hpp>
#include <Engine/Managers/SignalManager/SignalManager.hpp>
#include <Engine/Managers/SystemDisplay/SystemDisplay.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>

#include <GuiBase/SelectionManager/SelectionManager.hpp>
#include <GuiBase/Utils/PickingManager.hpp>

#include <MeshPaintSystem.hpp>
#include <UI/MeshPaintUI.h>
#include "ui_MeshPaintUI.h"

namespace MeshPaintPlugin
{

    MeshPaintPluginC::MeshPaintPluginC()
        : m_selectionManager(nullptr)
        , m_PickingManager(nullptr)
        , m_system(nullptr)
        , m_paintColor(Ra::Core::Color(1.0, 0.0, 0.0, 1.0))
        , m_isPainting(false)
    {
        m_widget = new MeshPaintUI();
        QColor color;
        color.setRed( m_paintColor(0) * 255 );
        color.setGreen( m_paintColor(1) * 255 );
        color.setBlue( m_paintColor(2) * 255 );
        color.setAlpha( m_paintColor(3) * 255 );
        m_widget->ui->changeColor_pb->setPalette( QPalette(color) );
    }

    MeshPaintPluginC::~MeshPaintPluginC()
    {
    }

    void MeshPaintPluginC::registerPlugin(const Ra::PluginContext& context)
    {
        // register selection context
        m_selectionManager = context.m_selectionManager;
        m_PickingManager = context.m_pickingManager;
        m_system = new MeshPaintSystem;
        context.m_engine->registerSystem( "MeshPaintSystem", m_system );
        connect( m_selectionManager, &Ra::GuiBase::SelectionManager::currentChanged, this, &MeshPaintPluginC::onCurrentChanged );
        connect( m_widget, &MeshPaintUI::paintColor, this, &MeshPaintPluginC::activePaintColor );
        connect( m_widget, &MeshPaintUI::colorChanged, this, &MeshPaintPluginC::changePaintColor );
    }

    bool MeshPaintPluginC::doAddWidget( QString &name )
    {
        name = "MeshPaint";
        return true;
    }

    QWidget* MeshPaintPluginC::getWidget()
    {
        return m_widget;
    }

    bool MeshPaintPluginC::doAddMenu()
    {
        return false;
    }

    QMenu* MeshPaintPluginC::getMenu()
    {
        return nullptr;
    }

    bool MeshPaintPluginC::doAddAction( int& nb )
    {
        nb = 0;
        return false;
    }

    QAction* MeshPaintPluginC::getAction( int id )
    {
        return nullptr;
    }

    void MeshPaintPluginC::activePaintColor( bool on )
    {
        m_isPainting = on;
        m_system->startPaintMesh( on );
    }

    void MeshPaintPluginC::changePaintColor( const QColor &color )
    {
        m_paintColor = Ra::Core::Color( Scalar(color.red()) / 255,
                                        Scalar(color.green()) / 255,
                                        Scalar(color.blue()) / 255, 1.0 );
    }

    void MeshPaintPluginC::onCurrentChanged( const QModelIndex& current, const QModelIndex& prev )
    {
        if ( m_isPainting && Ra::Core::Index::Invalid() != m_selectionManager->currentItem().m_roIndex )
        {
            m_system->paintMesh( m_PickingManager->getCurrent(), m_paintColor );
        }
    }

}
