#include <AnimationPlugin.hpp>

#include <QAction>
#include <QIcon>
#include <QToolBar>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Managers/SignalManager/SignalManager.hpp>
#include <GuiBase/SelectionManager/SelectionManager.hpp>
#include <AnimationSystem.hpp>

#include <UI/AnimationUI.h>
#include "ui_AnimationUI.h"

namespace AnimationPlugin
{

    AnimationPluginC::AnimationPluginC() :m_system(nullptr)
    {
        m_widget = new AnimationUI();
        connect( m_widget, &AnimationUI::toggleXray,              this, &AnimationPluginC::toggleXray );
        connect( m_widget, &AnimationUI::showSkeleton,            this, &AnimationPluginC::toggleSkeleton );
        connect( m_widget, &AnimationUI::animationID,             this, &AnimationPluginC::setAnimation );
        connect( m_widget, &AnimationUI::toggleAnimationTimeStep, this, &AnimationPluginC::toggleAnimationTimeStep );
        connect( m_widget, &AnimationUI::animationSpeed,          this, &AnimationPluginC::setAnimationSpeed );
        connect( m_widget, &AnimationUI::toggleSlowMotion,        this, &AnimationPluginC::toggleSlowMotion );
        connect( m_widget, &AnimationUI::play,                    this, &AnimationPluginC::play );
        connect( m_widget, &AnimationUI::pause,                   this, &AnimationPluginC::pause );
        connect( m_widget, &AnimationUI::step,                    this, &AnimationPluginC::step );
        connect( m_widget, &AnimationUI::stop,                    this, &AnimationPluginC::reset );
    }

    AnimationPluginC::~AnimationPluginC()
    {
    }

    void AnimationPluginC::registerPlugin(const Ra::PluginContext& context)
    {
        m_system = new AnimationSystem;
        context.m_engine->registerSystem( "AnimationSystem", m_system );
        context.m_engine->getSignalManager()->m_frameEndCallbacks.push_back(
                std::bind(&AnimationPluginC::updateAnimTime, this)
        );
        m_selectionManager = context.m_selectionManager;
    }

    bool AnimationPluginC::doAddWidget( QString &name )
    {
        name = "Animation";
        return true;
    }

    QWidget* AnimationPluginC::getWidget()
    {
        return m_widget;
    }

    bool AnimationPluginC::doAddMenu()
    {
        return false;
    }

    QMenu* AnimationPluginC::getMenu()
    {
        return nullptr;
    }

    bool AnimationPluginC::doAddAction( int& nb )
    {
        nb = 4;
        return true;
    }

    QAction* AnimationPluginC::getAction( int id )
    {
        switch (id) {
        case 0:
            return m_widget->ui->actionXray;
        case 1:
            return m_widget->ui->actionPlay;
        case 2:
            return m_widget->ui->actionStep;
        case 3:
            return m_widget->ui->actionStop;
        default:
            return nullptr;
        }
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

    void AnimationPluginC::updateAnimTime()
    {
        m_widget->updateTime( m_system->getTime( m_selectionManager->currentItem()));
    }
}
