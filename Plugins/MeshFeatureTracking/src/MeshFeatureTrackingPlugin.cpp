#include <MeshFeatureTrackingPlugin.hpp>

#include <QAction>
#include <QIcon>
#include <QToolBar>

#include <Engine/Entity/Entity.hpp>
#include <Engine/Managers/EntityManager/EntityManager.hpp>
#include <Engine/Managers/SignalManager/SignalManager.hpp>
#include <Engine/Managers/SystemDisplay/SystemDisplay.hpp>
#include <Engine/RadiumEngine.hpp>

#include <GuiBase/SelectionManager/SelectionManager.hpp>
#include <GuiBase/Utils/FeaturePickingManager.hpp>

#include <MeshFeatureTrackingComponent.hpp>

#include <UI/MeshFeatureTrackingUI.h>
#include "ui_MeshFeatureTrackingUI.h"

namespace MeshFeatureTrackingPlugin
{

    MeshFeatureTrackingPluginC::MeshFeatureTrackingPluginC()
        : m_selectionManager(nullptr)
        , m_featurePickingManager(nullptr)
    {
        m_widget = new MeshFeatureTrackingUI();
    }

    MeshFeatureTrackingPluginC::~MeshFeatureTrackingPluginC()
    {
    }

    void MeshFeatureTrackingPluginC::registerPlugin(const Ra::PluginContext& context)
    {
        // register system
        context.m_engine->getSignalManager()->m_frameEndCallbacks.push_back(
                std::bind(&MeshFeatureTrackingPluginC::update, this)
        );
        // create sphere entity
        m_entity = context.m_engine->getEntityManager()->createEntity( "FeatureTrackingEntity" );
        auto component = new MeshFeatureTrackingComponent( "TrackingSphere" );
        m_entity->addComponent( component );
        m_entity->idx = Ra::Engine::SystemEntity::getInstance()->idx; // hack to avoid selection through tree view
        component->initialize();
        // register selection context
        m_selectionManager = context.m_selectionManager;
        m_featurePickingManager = context.m_featureManager;
        connect( m_selectionManager, &Ra::GuiBase::SelectionManager::currentChanged, this, &MeshFeatureTrackingPluginC::onCurrentChanged );
    }

    bool MeshFeatureTrackingPluginC::doAddWidget( QString &name )
    {
        return false;
    }

    QWidget* MeshFeatureTrackingPluginC::getWidget()
    {
        return nullptr;
    }

    bool MeshFeatureTrackingPluginC::doAddMenu()
    {
        return false;
    }

    QMenu* MeshFeatureTrackingPluginC::getMenu()
    {
        return nullptr;
    }

    bool MeshFeatureTrackingPluginC::doAddAction( int& nb )
    {
        nb = 4;
        return true;
    }

    QAction* MeshFeatureTrackingPluginC::getAction( int id )
    {
        switch (id) {
        case 0:
            return m_widget->ui->actionXray;
        case 1:
            return m_widget->ui->actionPlay;
        case 2:
            return m_widget->ui->actionStep;
        case 3:
            return m_widget->ui->actionStop;
        default:
            return nullptr;
        }
    }

    bool MeshFeatureTrackingPluginC::doAddFeatureTrackingWidget()
    {
        return true;
    }

    QWidget* MeshFeatureTrackingPluginC::getFeatureTrackingWidget()
    {
        return m_widget;
    }

    void MeshFeatureTrackingPluginC::onCurrentChanged( const QModelIndex& current, const QModelIndex& prev )
    {
        auto data = m_featurePickingManager->getFeatureData();
        auto cmp = static_cast<MeshFeatureTrackingComponent*>(m_entity->getComponents()[0].get());
        if (data.m_featureType != Ra::Engine::Renderer::RO)
        {
            cmp->setData( data );
        }
        else
        {
            cmp->setData( Ra::Gui::FeatureData() );
        }
    }

    void MeshFeatureTrackingPluginC::update()
    {
        auto cmp = static_cast<MeshFeatureTrackingComponent*>(m_entity->getComponents()[0].get());
        cmp->update();
        m_widget->updateTracking( cmp->m_data, cmp->getFeaturePosition(), cmp->getFeatureVector() );
    }
}
