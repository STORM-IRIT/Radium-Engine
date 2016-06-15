#include <FancyMeshPlugin.hpp>

#include <Engine/RadiumEngine.hpp>

#include <FancyMeshSystem.hpp>

namespace FancyMeshPlugin
{

    FancyMeshPluginC::~FancyMeshPluginC()
    {
    }

    void FancyMeshPluginC::registerPlugin( const Ra::PluginContext& context )
    {
        FancyMeshSystem* system = new FancyMeshSystem;
        context.m_engine->registerSystem( "FancyMeshSystem", system );
    }

    bool FancyMeshPluginC::doAddWidget( QString &name )
    {
        return false;
    }

    QWidget* FancyMeshPluginC::getWidget()
    {
        return nullptr;
    }

    bool FancyMeshPluginC::doAddMenu()
    {
        return false;
    }

    QMenu* FancyMeshPluginC::getMenu()
    {
        return nullptr;
    }
}
