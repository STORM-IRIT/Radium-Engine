#include <FancyMeshPlugin.hpp>

#include <Engine/RadiumEngine.hpp>

#include <FancyMeshSystem.hpp>

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
