#include <AnimationPlugin.hpp>

#include <QAction>
#include <QIcon>
#include <QToolBar>

#include <Engine/RadiumEngine.hpp>
#include <AnimationSystem.hpp>

#include <UI/AnimationUI.h>

namespace AnimationPlugin
{

    AnimationPluginC::AnimationPluginC() :m_system(nullptr){}

    AnimationPluginC::~AnimationPluginC()
    {
    }

    void AnimationPluginC::registerPlugin( Ra::Engine::RadiumEngine* engine )
    {
        m_system = new AnimationSystem;
        engine->registerSystem( "AnimationSystem", m_system );
    }

    bool AnimationPluginC::doAddWidget( QString &name )
    {
        name = "Animation";
        return true;
    }

    QWidget* AnimationPluginC::getWidget()
    {
        AnimationUI* widget = new AnimationUI();

        connect( widget, &AnimationUI::toggleXray,              this, &AnimationPluginC::toggleXray );
        connect( widget, &AnimationUI::showSkeleton,            this, &AnimationPluginC::toggleSkeleton );
        connect( widget, &AnimationUI::animationID,             this, &AnimationPluginC::setAnimation );
        connect( widget, &AnimationUI::toggleAnimationTimeStep, this, &AnimationPluginC::toggleAnimationTimeStep );
        connect( widget, &AnimationUI::animationSpeed,          this, &AnimationPluginC::setAnimationSpeed );
        connect( widget, &AnimationUI::toggleSlowMotion,        this, &AnimationPluginC::toggleSlowMotion );
        connect( widget, &AnimationUI::play,                    this, &AnimationPluginC::play );
        connect( widget, &AnimationUI::pause,                   this, &AnimationPluginC::pause );
        connect( widget, &AnimationUI::step,                    this, &AnimationPluginC::step );
        connect( widget, &AnimationUI::stop,                    this, &AnimationPluginC::reset );


        return widget;
    }

    bool AnimationPluginC::doAddMenu()
    {
        return false;
    }

    QMenu* AnimationPluginC::getMenu()
    {
        return nullptr;
    }

    void AnimationPluginC::toggleXray(bool on)
    {
        CORE_ASSERT(m_system, "System should be there ");
        m_system->setXray(on);
    }

    void AnimationPluginC::play()
    {
        CORE_ASSERT(m_system, "System should be there ");
        m_system->setPlaying( true );
    }

    void AnimationPluginC::pause()
    {
        CORE_ASSERT(m_system, "System should be there ");
        m_system->setPlaying(false);
    }

    void AnimationPluginC::step()
    {
        CORE_ASSERT(m_system, "System should be there ");
        pause();
        m_system->step();
    }

    void AnimationPluginC::reset()
    {
        CORE_ASSERT(m_system, "System should be there ");
        pause();
        m_system->reset();
    }

    void AnimationPluginC::toggleSkeleton( bool status ) {
        m_system->toggleSkeleton( status );
    }

    void AnimationPluginC::setAnimation( uint i ) {
        m_system->setAnimation( i );
    }

    void AnimationPluginC::toggleAnimationTimeStep( bool status ) {
        m_system->toggleAnimationTimeStep( status );
    }

    void AnimationPluginC::setAnimationSpeed( Scalar value ) {
        m_system->setAnimationSpeed( value );
    }

    void AnimationPluginC::toggleSlowMotion( bool status ) {
        m_system->toggleSlowMotion( status );
    }
}
