#include <AnimationPlugin.hpp>

#include <QAction>
#include <QIcon>
#include <QToolBar>

#include <Engine/RadiumEngine.hpp>
#include <AnimationSystem.hpp>

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
        QWidget* widget = new QWidget();
        QToolBar* tb = new QToolBar(widget);
        {
            QAction* toggleXray = new QAction( QIcon(":/Assets/Images/xray.png"), "Toggle Xray", widget );
            toggleXray->setCheckable(true);
            toggleXray->setChecked(m_system->isXrayOn());
            connect( toggleXray, &QAction::toggled, this, &AnimationPluginC::toggleXray);
            tb->addAction(toggleXray);
        }
        {
            QAction* play = new QAction( QIcon(":/Assets/Images/play.png"),"Play", widget );
            connect( play , &QAction::triggered, this, &AnimationPluginC::play);
            tb->addAction(play);
        }
        {
            QAction* pause = new QAction( QIcon(":/Assets/Images/pause.png"),"pause", widget );
            connect( pause , &QAction::triggered, this, &AnimationPluginC::pause);
            tb->addAction(pause);
        }
        {
            QAction* step = new QAction( "Step", widget );
            connect( step , &QAction::triggered, this, &AnimationPluginC::step);
            tb->addAction(step);
        }
        {
            QAction* reset = new QAction( "Reset", widget );
            connect( reset , &QAction::triggered, this, &AnimationPluginC::reset);
            tb->addAction(reset);
        }

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
}
