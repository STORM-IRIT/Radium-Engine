#include <PointCloudPlugin.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Managers/SignalManager/SignalManager.hpp>
#include <PointCloudSystem.hpp>

namespace PointCloudPlugin
{
    PointCloudPluginC::PointCloudPluginC() : m_system(nullptr)
    {
        m_widget = new PointCloudUI();
        connect( m_widget, &PointCloudUI::changeNeighSize, this, &PointCloudPluginC::changeNeighSize );
        connect( m_widget, &PointCloudUI::changeUseNormal, this, &PointCloudPluginC::changeUseNormal );
        connect( m_widget, &PointCloudUI::changeDepthThresh, this, &PointCloudPluginC::changeDepthThresh );
        connect( m_widget, &PointCloudUI::changeDepthCalc, this, &PointCloudPluginC::changeDepthCalc );
    }
    PointCloudPluginC::~PointCloudPluginC()
    {
    }

    void PointCloudPluginC::registerPlugin( const Ra::PluginContext& context )
    {
        m_system = new PointCloudSystem;
        context.m_engine->registerSystem( "PointCloudSystem", m_system );
        m_selectionManager = context.m_selectionManager;
    }

    bool PointCloudPluginC::doAddWidget( QString &name )
    {  
        name = "Point Cloud";
        return true;
    }

    QWidget* PointCloudPluginC::getWidget()
    {
        return m_widget;
    }

    bool PointCloudPluginC::doAddMenu()
    {
        return false;
    }

    QMenu* PointCloudPluginC::getMenu()
    {
        return nullptr;
    }

    bool PointCloudPluginC::doAddAction( int& nb )
    {
        nb = 0;
        return false;
    }

    QAction* PointCloudPluginC::getAction( int id )
    {
        return nullptr;
    }


    void PointCloudPluginC::changeNeighSize(int size)
    {
        CORE_ASSERT(m_system, "System should be there ");
        m_system->setNeighSize(size);
    }

    void PointCloudPluginC::changeUseNormal(bool useNormal)
    {
        CORE_ASSERT(m_system, "System should be there ");
        m_system->setUseNormal(useNormal);
    }
    void PointCloudPluginC::changeDepthThresh(double dThresh)
    {
        CORE_ASSERT(m_system, "System should be there ");
        m_system->setDepthThresh(dThresh);
    }

    void PointCloudPluginC::changeDepthCalc(int index)
    {
        CORE_ASSERT(m_system, "System should be there ");
        m_system->setDepthCalc(index);
    }
}
