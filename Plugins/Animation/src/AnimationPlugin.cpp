#include <AnimationPlugin.hpp>

#include <Engine/RadiumEngine.hpp>

#include <AnimationSystem.hpp>

namespace AnimationPlugin
{

    AnimationPluginC::~AnimationPluginC()
    {
        printf( "lolol" );
        fflush( stdout );
    }

    void AnimationPluginC::registerPlugin( Ra::Engine::RadiumEngine* engine )
    {
        AnimationSystem* system = new AnimationSystem;
        engine->registerSystem( "AnimationSystem", system );
    }

    bool AnimationPluginC::doAddWidget( QString &name )
    {
        return false;
    }

    QWidget* AnimationPluginC::getWidget()
    {
        return nullptr;
    }

    bool AnimationPluginC::doAddMenu()
    {
        return false;
    }

    QMenu* AnimationPluginC::getMenu()
    {
        return nullptr;
    }
}
