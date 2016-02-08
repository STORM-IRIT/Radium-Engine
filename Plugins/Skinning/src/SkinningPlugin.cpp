#include <SkinningPlugin.hpp>

#include <Engine/RadiumEngine.hpp>

#include <SkinningSystem.hpp>

namespace SkinningPlugin
{

    SkinningPluginC::~SkinningPluginC()
    {
    }

    void SkinningPluginC::registerPlugin( Ra::Engine::RadiumEngine* engine )
    {
        SkinningSystem* system = new SkinningSystem;
        engine->registerSystem( "SkinningSystem", system );
    }

    bool SkinningPluginC::doAddWidget( QString &name )
    {
        return false;
    }

    QWidget* SkinningPluginC::getWidget()
    {
        return nullptr;
    }

    bool SkinningPluginC::doAddMenu()
    {
        return false;
    }

    QMenu* SkinningPluginC::getMenu()
    {
        return nullptr;
    }
}
