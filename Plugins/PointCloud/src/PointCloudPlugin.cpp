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
        connect( m_widget, &PointCloudUI::changeShowPos, this, &PointCloudPluginC::changeShowPos );
        connect( m_widget, &PointCloudUI::changeDepthThresh, this, &PointCloudPluginC::changeDepthThresh );
        connect( m_widget, &PointCloudUI::changePlaneFit, this, &PointCloudPluginC::changePlaneFit );
        connect( m_widget, &PointCloudUI::changeRadius, this, &PointCloudPluginC::changeRadius );
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
    void PointCloudPluginC::changeShowPos(bool showPos)
    {
        CORE_ASSERT(m_system, "System should be there ");
        m_system->setShowPos(showPos);
    }
    void PointCloudPluginC::changePlaneFit(bool planeFit)
    {
        CORE_ASSERT(m_system, "System should be there ");
        m_system->setPlaneFit(planeFit);
    }
    void PointCloudPluginC::changeDepthThresh(double dThresh)
    {
        CORE_ASSERT(m_system, "System should be there ");
        m_system->setDepthThresh(dThresh);
    }
    void PointCloudPluginC::changeRadius(double radius)
    {
        CORE_ASSERT(m_system, "System should be there ");
        m_system->setRadius(radius);
    }
    void PointCloudPluginC::changeDepthCalc(int index)
    {
        CORE_ASSERT(m_system, "System should be there ");
        m_system->setDepthCalc(index);
    }
}
