#include "FancyMeshPlugin.hpp"

#include <Core/Log/Log.hpp>

namespace FancyMeshPlugin
{

    FancyMeshPlugin::~FancyMeshPlugin()
    {
    }

    void FancyMeshPlugin::registerPlugin()
    {
        LOG( logINFO ) << "Hello from registerPlugin()";
    }

    void FancyMeshPlugin::setupInterface()
    {
        LOG( logINFO ) << "Hello from setupInterface()";
    }

}
