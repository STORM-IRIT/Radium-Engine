#include "FancyMeshPlugin.hpp"

#include <QWidget>

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
    }

    bool FancyMeshPlugin::doAddWidget( QString &name )
    {
        name = "Fancy Mesh";
        return true;
    }

    QWidget* FancyMeshPlugin::getWidget()
    {
        return new QWidget();
    }

    bool FancyMeshPlugin::doAddMenu( QString &name )
    {
        name = "Fancy Mesh";
        return true;
    }
}
