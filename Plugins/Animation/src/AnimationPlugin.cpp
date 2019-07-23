#include <AnimationPlugin.hpp>

#include <QAction>
#include <QFileDialog>
#include <QIcon>
#include <QSettings>
#include <QToolBar>

#include <AnimationSystem.hpp>
#include <Engine/Managers/SignalManager/SignalManager.hpp>
#include <Engine/RadiumEngine.hpp>
#include <GuiBase/SelectionManager/SelectionManager.hpp>

#include "ui_AnimationUI.h"
#include <UI/AnimationUI.h>

namespace AnimationPlugin {

AnimationPluginC::AnimationPluginC() = default;

AnimationPluginC::~AnimationPluginC() = default;

void AnimationPluginC::registerPlugin( const Ra::Plugins::Context& context ) {
    QSettings settings;
    QString path = settings.value( "AnimDataDir" ).toString();
    if ( path.isEmpty() ) { path = QString( context.m_exportDir.c_str() ); }
    m_system = new AnimationSystem;
    context.m_engine->registerSystem( "AnimationSystem", m_system );
    context.m_engine->getSignalManager()->m_frameEndCallbacks.push_back(
        std::bind( &AnimationPluginC::updateAnimTime, this ) );
    m_selectionManager = context.m_selectionManager;

    connect( this, &AnimationPluginC::askForUpdate, &context, &Ra::Plugins::Context::askForUpdate );
    connect( this,
             &AnimationPluginC::setContinuousUpdate,
             &context,
             &Ra::Plugins::Context::setContinuousUpdate );
}

bool AnimationPluginC::doAddWidget( QString& name ) {
    name = "Animation";
    return true;
}

QWidget* AnimationPluginC::getWidget() {
    m_widget = new AnimationUI();
    connect( m_widget, &AnimationUI::toggleXray, this, &AnimationPluginC::toggleXray );
    connect( m_widget, &AnimationUI::showSkeleton, this, &AnimationPluginC::toggleSkeleton );
    connect( m_widget, &AnimationUI::animationID, this, &AnimationPluginC::setAnimation );
    connect( m_widget,
             &AnimationUI::toggleAnimationTimeStep,
             this,
             &AnimationPluginC::toggleAnimationTimeStep );
    connect( m_widget, &AnimationUI::animationSpeed, this, &AnimationPluginC::setAnimationSpeed );
    connect( m_widget, &AnimationUI::toggleSlowMotion, this, &AnimationPluginC::toggleSlowMotion );
    connect( m_widget, &AnimationUI::play, this, &AnimationPluginC::play );
    connect( m_widget, &AnimationUI::pause, this, &AnimationPluginC::pause );
    connect( m_widget, &AnimationUI::step, this, &AnimationPluginC::step );
    connect( m_widget, &AnimationUI::stop, this, &AnimationPluginC::reset );
    connect( m_widget, &AnimationUI::cacheFrame, this, &AnimationPluginC::cacheFrame );
    connect( m_widget, &AnimationUI::restoreFrame, this, &AnimationPluginC::restoreFrame );
    connect( m_widget, &AnimationUI::changeDataDir, this, &AnimationPluginC::changeDataDir );
    return m_widget;
}

bool AnimationPluginC::doAddMenu() {
    return false;
}

QMenu* AnimationPluginC::getMenu() {
    return nullptr;
}

bool AnimationPluginC::doAddAction( int& nb ) {
    nb = 4;
    return true;
}

QAction* AnimationPluginC::getAction( int id ) {
    switch ( id )
    {
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

void AnimationPluginC::toggleXray( bool on ) {
    CORE_ASSERT( m_system, "System should be there " );
    m_system->setXray( on );
    askForUpdate();
}

void AnimationPluginC::play() {
    CORE_ASSERT( m_system, "System should be there " );
    m_system->play( true );
    emit setContinuousUpdate( true );
    askForUpdate();
}

void AnimationPluginC::pause() {
    CORE_ASSERT( m_system, "System should be there " );
    m_system->play( false );
    emit setContinuousUpdate( false );
}

void AnimationPluginC::step() {
    CORE_ASSERT( m_system, "System should be there " );
    pause();
    m_system->step();
    askForUpdate();
}

void AnimationPluginC::reset() {
    CORE_ASSERT( m_system, "System should be there " );
    pause();
    m_system->reset();
    askForUpdate();
}

void AnimationPluginC::toggleSkeleton( bool status ) {
    m_system->toggleSkeleton( status );
    askForUpdate();
}

void AnimationPluginC::setAnimation( uint i ) {
    m_system->setAnimation( i );
    askForUpdate();
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

void AnimationPluginC::updateAnimTime() {
    if (m_widget) {
        m_widget->setMaxFrame( m_system->getMaxFrame() );
        m_widget->updateTime( m_system->getTime( m_selectionManager->currentItem() ) );
        m_widget->updateFrame( m_system->getAnimFrame() );
    }
    askForUpdate();
}

void AnimationPluginC::cacheFrame() {
    m_system->cacheFrame( m_dataDir );
}

void AnimationPluginC::restoreFrame( int frame ) {
    if ( m_system->restoreFrame( m_dataDir, frame ) )
    {
        m_widget->frameLoaded( frame );
        askForUpdate();
    }
}

void AnimationPluginC::changeDataDir() {
    QSettings settings;
    QString path = settings.value( "AnimDataDir", QDir::homePath() ).toString();
    path         = QFileDialog::getExistingDirectory( nullptr, "Animation Data Dir", path );
    if ( !path.isEmpty() )
    {
        settings.setValue( "AnimDataDir", path );
        m_dataDir = path.toStdString();
    }
}

} // namespace AnimationPlugin
