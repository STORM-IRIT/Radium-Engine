#include <SkinningPlugin.hpp>

#include <Engine/RadiumEngine.hpp>

#include <SkinningSystem.hpp>

namespace SkinningPlugin
{

    SkinningPluginC::~SkinningPluginC()
    {
    }

    void SkinningPluginC::registerPlugin( const Ra::PluginContext& context )
    {
        SkinningSystem* system = new SkinningSystem;
        context.m_engine->registerSystem( "SkinningSystem", system );
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
