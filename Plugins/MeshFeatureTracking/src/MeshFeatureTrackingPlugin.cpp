#include <MeshFeatureTrackingPlugin.hpp>

#include <QAction>
#include <QIcon>
#include <QToolBar>

#include <Engine/Entity/Entity.hpp>
#include <Engine/Managers/EntityManager/EntityManager.hpp>
#include <Engine/Managers/SignalManager/SignalManager.hpp>
#include <Engine/RadiumEngine.hpp>

#include <GuiBase/SelectionManager/SelectionManager.hpp>
#include <GuiBase/Utils/PickingManager.hpp>

#include <MeshFeatureTrackingComponent.hpp>

#include "ui_MeshFeatureTrackingUI.h"
#include <UI/MeshFeatureTrackingUI.h>

namespace MeshFeatureTrackingPlugin {

MeshFeatureTrackingPluginC::MeshFeatureTrackingPluginC() {}

MeshFeatureTrackingPluginC::~MeshFeatureTrackingPluginC() {}

void MeshFeatureTrackingPluginC::registerPlugin( const Ra::Plugins::Context& context ) {
    // register system
    context.m_engine->getSignalManager()->m_frameEndCallbacks.push_back(
        std::bind( &MeshFeatureTrackingPluginC::update, this ) );
    // create sphere component
    m_component = new MeshFeatureTrackingComponent( "TrackingSphere" );
    m_component->initialize();
    // register selection context
    m_selectionManager = context.m_selectionManager;
    m_PickingManager   = context.m_pickingManager;
    connect( m_selectionManager,
             &Ra::GuiBase::SelectionManager::currentChanged,
             this,
             &MeshFeatureTrackingPluginC::onCurrentChanged );
    connect( this,
             &MeshFeatureTrackingPluginC::askForUpdate,
             &context,
             &Ra::Plugins::Context::askForUpdate );
}

bool MeshFeatureTrackingPluginC::doAddWidget( QString& name ) {
    name = "MeshFeatureTracking";
    return true;
}

QWidget* MeshFeatureTrackingPluginC::getWidget() {
    if ( m_widget == nullptr )
    {
        m_widget = new MeshFeatureTrackingUI();
        connect( m_widget,
                 &MeshFeatureTrackingUI::vertexIdChanged,
                 this,
                 &MeshFeatureTrackingPluginC::vertexIdChanged );
        connect( m_widget,
                 &MeshFeatureTrackingUI::triangleIdChanged,
                 this,
                 &MeshFeatureTrackingPluginC::triangleIdChanged );
    }
    return m_widget;
}

bool MeshFeatureTrackingPluginC::doAddMenu() {
    return false;
}

QMenu* MeshFeatureTrackingPluginC::getMenu() {
    return nullptr;
}

bool MeshFeatureTrackingPluginC::doAddAction( int& nb ) {
    nb = 0;
    return false;
}

QAction* MeshFeatureTrackingPluginC::getAction( int id ) {
    return nullptr;
}

void MeshFeatureTrackingPluginC::onCurrentChanged( const QModelIndex& current,
                                                   const QModelIndex& prev ) {
    m_component->setData( m_PickingManager->getCurrent() );
    if ( m_widget )
    {
        m_widget->setMaxV( m_component->getMaxV() );
        m_widget->setMaxT( m_component->getMaxT() );
    }
}

void MeshFeatureTrackingPluginC::update() {
    m_component->update();
    if ( m_widget )
    {
        m_widget->updateTracking( m_component->getFeatureData(),
                                  m_component->getFeaturePosition(),
                                  m_component->getFeatureVector() );
    }
}
void MeshFeatureTrackingPluginC::vertexIdChanged( int idx ) {
    m_component->setVertexIdx( idx );
    update();
    askForUpdate();
}

void MeshFeatureTrackingPluginC::triangleIdChanged( int idx ) {
    m_component->setTriangleIdx( idx );
    update();
    askForUpdate();
}

} // namespace MeshFeatureTrackingPlugin
