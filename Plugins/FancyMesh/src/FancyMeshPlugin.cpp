#include "FancyMeshPlugin.hpp"

#include <Core/Log/Log.hpp>
#include <Engine/RadiumEngine.hpp>

#include "FancyMeshSystem.hpp"

namespace FancyMeshPlugin
{

    FancyMeshPlugin::~FancyMeshPlugin()
    {
    }

    void FancyMeshPlugin::registerPlugin( Ra::Engine::RadiumEngine* engine )
    {
        FancyMeshSystem* system = new FancyMeshSystem;
        engine->registerSystem( "FancyMeshSystem", system );
        LOG( logINFO ) << "Hello from registerPlugin()";
    }

    void FancyMeshPlugin::setupInterface()
    {
        LOG( logINFO ) << "Hello from setupInterface()";
    }

}
