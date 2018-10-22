#include <PostSubdivPlugin.hpp>

#include <Engine/RadiumEngine.hpp>

#include <GuiBase/SelectionManager/SelectionManager.hpp>
#include <PostSubdivSystem.hpp>

#include "ui_PostSubdivUI.h"

namespace PostSubdivPlugin {

PostSubdivPluginC::~PostSubdivPluginC() {}

void PostSubdivPluginC::registerPlugin( const Ra::PluginContext& context ) {
    m_system = new PostSubdivSystem;
    m_selectionManager = context.m_selectionManager;
    context.m_engine->registerSystem( "PostSubdivSystem", m_system, -10 );
    connect( m_selectionManager, &Ra::GuiBase::SelectionManager::currentChanged, this,
             &PostSubdivPluginC::onCurrentChanged );
}

bool PostSubdivPluginC::doAddWidget( QString& name ) {
    name = "PostSubdiv";
    return true;
}

QWidget* PostSubdivPluginC::getWidget() {
    m_widget = new PostSubdivUI;
    connect( m_widget, &PostSubdivUI::subdivMethodChanged, this,
             &PostSubdivPluginC::onSubdivMethodChanged );
    connect( m_widget, &PostSubdivUI::subdivIterChanged, this,
             &PostSubdivPluginC::onSubdivIterChanged );
    return m_widget;
}

bool PostSubdivPluginC::doAddMenu() {
    return false;
}

QMenu* PostSubdivPluginC::getMenu() {
    return nullptr;
}

bool PostSubdivPluginC::doAddAction( int& nb ) {
    nb = 0;
    return false;
}

QAction* PostSubdivPluginC::getAction( int id ) {
    return nullptr;
}

void PostSubdivPluginC::onCurrentChanged( const QModelIndex& current, const QModelIndex& prev ) {
    Ra::Engine::ItemEntry it = m_selectionManager->currentItem();
    if ( it.m_entity )
    {
        auto comps = m_system->getEntityComponents( it.m_entity );
        if ( comps.size() != 0 )
        {
            setCurrent( it, static_cast<PostSubdivPlugin::PostSubdivComponent*>( comps[0] ) );
        } else
        { setCurrent( it, nullptr ); }
    } else
    { setCurrent( it, nullptr ); }
}

void PostSubdivPluginC::setCurrent( const Ra::Engine::ItemEntry& entry,
                                    PostSubdivComponent* comp ) {
    m_current = comp;
    if ( m_current )
    {
        m_widget->ui->m_subdivMethod->setEnabled( true );
        m_widget->ui->m_subdivMethod->setCurrentIndex( int( m_current->getSubdivMethod() ) );
        m_widget->ui->m_subdivIter->setEnabled( true );
        m_widget->ui->m_subdivIter->setValue( m_current->getSubdivIter() );
    } else
    {
        m_widget->ui->m_subdivMethod->setEnabled( false );
        m_widget->ui->m_subdivIter->setEnabled( false );
    }
}

void PostSubdivPluginC::onSubdivMethodChanged( int method ) {
    CORE_ASSERT( m_current, "should be disabled" );
    CORE_ASSERT( method >= 0 && method < 2, "Invalid PostSubdiv Type" );
    m_current->setSubdivMethod( PostSubdivComponent::SubdivMethod( method ) );
}

void PostSubdivPluginC::onSubdivIterChanged( int iter ) {
    CORE_ASSERT( m_current, "should be disabled" );
    CORE_ASSERT( iter >= 0 && iter < 4, "Invalid PostSubdiv Iter" );
    m_current->setSubdivIter( iter );
}

} // namespace PostSubdivPlugin
