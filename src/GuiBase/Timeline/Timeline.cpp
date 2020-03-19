#include "ui_Timeline.h"
#include <GuiBase/Timeline/Timeline.h>

#include <QDesktopWidget>
#include <QEvent>
#include <QMessageBox>
#include <QPainter>
#include <QTimer>
#include <QWheelEvent>

#include <Core/Animation/KeyFramedValueInterpolators.hpp>
#include <Core/Utils/Log.hpp>

#include <Engine/Component/Component.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Managers/SignalManager/SignalManager.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>

#include <GuiBase/Timeline/HelpDialog.hpp>

using namespace Ra::Core::Utils;

namespace Ra::GuiBase {

Timeline::Timeline( QWidget* parent ) : QDialog( parent ), ui( new Ui::Timeline ) {
    ui->setupUi( this );

    // set sizePolicy to allow zoom in scrollArea
    ui->scrollAreaWidgetContents->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );
    // first draw of ruler with current width (default in Timeline.ui) of dialog
    ui->m_scrollArea->onDrawRuler( width() - 2 ); // left/right border width = 2 *1 pixel

    // --- SET INTERNAL REFERENCES ---
    ui->frame_timescale->setScrollArea( ui->m_scrollArea );
    ui->frame_selector->setTimelineUi( ui );

    // --- CREATE INTERNAL CONNECTIONS ---
    connect( ui->frame_selector,
             &TimelineFrameSelector::cursorChanged,
             this,
             &Timeline::updateKeyFrames );
    connect( ui->frame_selector,
             &TimelineFrameSelector::keyFrameAdded,
             this,
             &Timeline::onAddingKeyFrame );
    connect( ui->frame_selector,
             &TimelineFrameSelector::keyFrameDeleted,
             this,
             &Timeline::onRemovingKeyFrame );
    connect( ui->frame_selector,
             &TimelineFrameSelector::keyFrameChanged,
             this,
             &Timeline::onChangingKeyFrame );
    connect( ui->frame_selector,
             &TimelineFrameSelector::keyFrameMoved,
             this,
             &Timeline::onMovingKeyFrame );
    connect( ui->frame_selector,
             &TimelineFrameSelector::keyFramesMoved,
             this,
             &Timeline::onMovingKeyFrames );

    // --- CONNECT INTERNAL SIGNALS TO EXTERNAL SIGNALS ---
    connect( ui->toolButton_playPause, &QToolButton::toggled, this, &Timeline::playClicked );
    connect(
        ui->frame_selector, &TimelineFrameSelector::cursorChanged, this, &Timeline::cursorChanged );
    connect(
        ui->frame_selector, &TimelineFrameSelector::startChanged, this, &Timeline::startChanged );
    connect( ui->frame_selector, &TimelineFrameSelector::endChanged, this, &Timeline::endChanged );
    connect( ui->frame_selector,
             &TimelineFrameSelector::durationChanged,
             this,
             &Timeline::durationChanged );

    // --- DEAL WITH OBJET REMOVAL ---
    Ra::Engine::RadiumEngine::getInstance()
        ->getSignalManager()
        ->m_entityDestroyedCallbacks.push_back( [=]( const Ra::Engine::ItemEntry& entry ) {
            auto it = std::find_if(
                m_entityKeyFrames.begin(), m_entityKeyFrames.end(), [entry]( const auto& frames ) {
                    return entry.m_entity == frames.first;
                } );
            if ( it != m_entityKeyFrames.end() ) { m_entityKeyFrames.erase( it ); }
        } );
    Ra::Engine::RadiumEngine::getInstance()
        ->getSignalManager()
        ->m_componentRemovedCallbacks.push_back( [=]( const Ra::Engine::ItemEntry& entry ) {
            auto it = std::find_if(
                m_componentKeyFrames.begin(),
                m_componentKeyFrames.end(),
                [entry]( const auto& frames ) { return entry.m_component == frames.first; } );
            if ( it != m_componentKeyFrames.end() ) { m_componentKeyFrames.erase( it ); }
        } );
    Ra::Engine::RadiumEngine::getInstance()->getSignalManager()->m_roRemovedCallbacks.push_back(
        [=]( const Ra::Engine::ItemEntry& entry ) {
            auto it = std::find_if(
                m_renderObjectKeyFrames.begin(),
                m_renderObjectKeyFrames.end(),
                [entry]( const auto& frames ) { return entry.m_roIndex == frames.first; } );
            if ( it != m_renderObjectKeyFrames.end() ) { m_renderObjectKeyFrames.erase( it ); }
        } );
}

Timeline::~Timeline() {
    delete ui;
}

void Timeline::selectionChanged( const Ra::Engine::ItemEntry& ent ) {
    // enables ui if any keyframe
    auto enableUI = [=]( bool enable ) {
        ui->comboBox_attribute->setEnabled( enable );
        ui->toolButton_keyFrame->setEnabled( enable );
        ui->m_nbKeyFramesSpin->setEnabled( enable );
        ui->m_removeKeyFrameButton->setEnabled( enable );
        ui->pushButton_editAttribute->setEnabled( enable );
    };
    ui->comboBox_attribute->blockSignals( true );
    ui->comboBox_attribute->clear();
    ui->comboBox_attribute->blockSignals( false );
    enableUI( false );
    onClearKeyFrames();
    m_current = Ra::Core::Animation::KeyFramedValueController();
    // collect keyframes names and display times if first one
    static auto registerFrames = [=]( const Ra::Core::Animation::KeyFramedValueController& frame,
                                      const std::string& prefix ) {
        ui->comboBox_attribute->addItem( QString( ( prefix + frame.m_name ).c_str() ) );
        if ( ui->comboBox_attribute->count() == 1 )
        {
            const auto times = frame.m_value->getTimes();
            for ( const auto& t : times )
            {
                ui->frame_selector->onAddingKeyFrame( t, false );
            }
        }
    };
    // register keyframes for the Entity
    if ( ent.m_entity == nullptr ) { return; }
    const std::string& entityName = ent.m_entity->getName();
    auto entityFrames             = m_entityKeyFrames.find( ent.m_entity );
    if ( entityFrames != m_entityKeyFrames.end() )
    {
        if ( entityFrames->second.size() > 0 ) { m_current = entityFrames->second.front(); }
        for ( const auto& frames : entityFrames->second )
        {
            registerFrames( frames, entityName + "::" );
        }
    }
    // register keyframes for the Component
    if ( ent.m_component == nullptr )
    {
        enableUI( ui->comboBox_attribute->count() > 0 );
        return;
    }
    const std::string& compName = ent.m_component->getName();
    auto componentFrames        = m_componentKeyFrames.find( ent.m_component );
    if ( componentFrames != m_componentKeyFrames.end() )
    {
        if ( ui->comboBox_attribute->count() == 0 && componentFrames->second.size() > 0 )
        { m_current = componentFrames->second.front(); }
        for ( const auto& frames : componentFrames->second )
        {
            registerFrames( frames, entityName + "::" + compName + "::" );
        }
    }
    // register keyframes for the RenderObject
    if ( ent.m_roIndex == Ra::Core::Utils::Index::Invalid() )
    {
        enableUI( ui->comboBox_attribute->count() > 0 );
        return;
    }
    const auto& roMngr = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager();
    const std::string roName =
        roMngr->getRenderObject( ent.m_roIndex )->getName() + "_" + std::to_string( ent.m_roIndex );
    auto renderObjectFrames = m_renderObjectKeyFrames.find( ent.m_roIndex );
    if ( renderObjectFrames != m_renderObjectKeyFrames.end() )
    {
        if ( ui->comboBox_attribute->count() > 0 && renderObjectFrames->second.size() > 0 )
        { m_current = renderObjectFrames->second.front(); }
        for ( const auto& frames : renderObjectFrames->second )
        {
            registerFrames( frames, entityName + "::" + compName + "::" + roName + "::" );
        }
    }
    if ( ui->comboBox_attribute->count() > 0 )
    {
        enableUI( true );
        ui->comboBox_attribute->setCurrentIndex( 0 );
    }
}

void Timeline::registerKeyFramedValue(
    Ra::Engine::Entity* ent,
    std::string&& name,
    Ra::Core::Animation::KeyFramedValueBase* value,
    Ra::Core::Animation::KeyFramedValueController::InsertCallback inserter,
    Ra::Core::Animation::KeyFramedValueController::UpdateCallback updater ) {
    auto& values = m_entityKeyFrames[ent];
    auto it      = std::find_if( values.begin(), values.end(), [&name]( const auto& frame ) {
        return frame.m_name == name;
    } );
    if ( it != values.end() )
    {
        LOG( logWARNING ) << "[Timeline] Already existing KeyFramedValue: " << name
                          << "\" on Entity \"" << ent->getName() << "\". Will not register."
                          << std::endl;
        return;
    }
    values.push_back( Ra::Core::Animation::KeyFramedValueController(
        value, std::move( name ), inserter, updater ) );
    selectionChanged( Engine::ItemEntry( ent ) );
}

void Timeline::unregisterKeyFramedValue( Ra::Engine::Entity* ent, const std::string& name ) {
    auto& values = m_entityKeyFrames[ent];
    auto it      = std::find_if( values.begin(), values.end(), [&name]( const auto& frame ) {
        return frame.m_name == name;
    } );
    if ( it != values.end() ) { values.erase( it ); }
}

void Timeline::registerKeyFramedValue(
    Ra::Engine::Component* comp,
    std::string&& name,
    Ra::Core::Animation::KeyFramedValueBase* value,
    Ra::Core::Animation::KeyFramedValueController::InsertCallback inserter,
    Ra::Core::Animation::KeyFramedValueController::UpdateCallback updater ) {
    auto& values = m_componentKeyFrames[comp];
    auto it      = std::find_if( values.begin(), values.end(), [name]( const auto& frame ) {
        return frame.m_name == name;
    } );
    if ( it != values.end() )
    {
        LOG( logWARNING ) << "[Timeline] Already existing KeyFramedValue: \"" << name
                          << "\" on Component \"" << comp->getName() << "\". Will not register."
                          << std::endl;
        return;
    }
    values.push_back( Ra::Core::Animation::KeyFramedValueController(
        value, std::move( name ), inserter, updater ) );
    selectionChanged( Engine::ItemEntry( comp->getEntity(), comp ) );
}

void Timeline::unregisterKeyFramedValue( Ra::Engine::Component* comp, const std::string& name ) {
    auto& values = m_componentKeyFrames[comp];
    auto it      = std::find_if( values.begin(), values.end(), [&name]( const auto& frame ) {
        return frame.m_name == name;
    } );
    if ( it != values.end() ) { values.erase( it ); }
}

void Timeline::registerKeyFramedValue(
    Ra::Core::Utils::Index roIdx,
    std::string&& name,
    Ra::Core::Animation::KeyFramedValueBase* value,
    Ra::Core::Animation::KeyFramedValueController::InsertCallback inserter,
    Ra::Core::Animation::KeyFramedValueController::UpdateCallback updater ) {
    auto& values = m_renderObjectKeyFrames[roIdx];
    auto it      = std::find_if( values.begin(), values.end(), [&name]( const auto& frame ) {
        return frame.m_name == name;
    } );
    if ( it != values.end() )
    {
        LOG( logWARNING ) << "[Timeline] Already existing KeyFramedValue: " << name
                          << "\" on RenderObject \"" << roIdx << "\". Will not register."
                          << std::endl;
        return;
    }
    values.push_back( Ra::Core::Animation::KeyFramedValueController(
        value, std::move( name ), inserter, updater ) );
    auto roMgr = Engine::RadiumEngine::getInstance()->getRenderObjectManager();
    auto RO    = roMgr->getRenderObject( roIdx );
    auto comp  = RO->getComponent();
    selectionChanged( Engine::ItemEntry( comp->getEntity(), comp, roIdx ) );
}

void Timeline::unregisterKeyFramedValue( Ra::Core::Utils::Index roIdx, const std::string& name ) {
    auto& values = m_renderObjectKeyFrames[roIdx];
    auto it      = std::find_if( values.begin(), values.end(), [&name]( const auto& frame ) {
        return frame.m_name == name;
    } );
    if ( it != values.end() ) { values.erase( it ); }
}

Scalar Timeline::getTime() const {
    return Scalar( ui->m_cursorSpin->value() );
}

void Timeline::onChangeStart( Scalar time ) {
    ui->frame_selector->onChangeStart( time, false );
}

void Timeline::onChangeEnd( Scalar time ) {
    ui->frame_selector->onChangeEnd( time, false );
}

void Timeline::onChangeDuration( Scalar time ) {
    ui->frame_selector->onChangeDuration( time, false );
}

void Timeline::onChangeCursor( Scalar time ) {
    ui->frame_selector->onChangeCursor( time, false );
    updateKeyFrames( time );
}

void Timeline::onSetPlay( bool play ) {
    ui->toolButton_playPause->setChecked( play );
}

// todo : ctrlDown = shiftDown = false, on focus event (initialize state)
void Timeline::resizeEvent( QResizeEvent* event ) {
    ui->m_scrollArea->onDrawRuler( event->size().width() - 2 );
}

void Timeline::on_pingPong_toggled( bool checked ) {
    emit setPingPong( checked );
}

void Timeline::updateKeyFrames( Scalar time ) {
    for ( auto& KF : m_entityKeyFrames )
    {
        for ( auto& kf : KF.second )
        {
            kf.updateKeyFrame( time );
        }
    }
    for ( auto& KF : m_componentKeyFrames )
    {
        for ( auto& kf : KF.second )
        {
            kf.updateKeyFrame( time );
        }
    }
    for ( auto& KF : m_renderObjectKeyFrames )
    {
        for ( auto& kf : KF.second )
        {
            kf.updateKeyFrame( time );
        }
    }
}

void Timeline::onClearKeyFrames() {
    ui->frame_selector->onClearKeyFrames();
}

void Timeline::onAddingKeyFrame( Scalar time ) {
    if ( m_current.m_value )
    {
        m_current.insertKeyFrame( time );
        emit keyFrameAdded( time );
    }
}

void Timeline::onRemovingKeyFrame( size_t i ) {
    if ( m_current.m_value )
    {
        const Scalar time = m_current.m_value->getTimes()[i];
        if ( m_current.m_value->removeKeyFrame( i ) )
        {
            m_current.updateKeyFrame( time );
            emit keyFrameDeleted( i );
        }
        else
        {
            LOG( logWARNING ) << "[Timeline] Error: Cannot remove keyFrame at "
                              << Scalar( ui->m_cursorSpin->value() );
        }
    }
}

void Timeline::onChangingKeyFrame( size_t i ) {
    if ( m_current.m_value )
    {
        Scalar time = m_current.m_value->getTimes()[i];
        m_current.insertKeyFrame( time );
        emit keyFrameChanged( i );
    }
}

void Timeline::onMovingKeyFrame( size_t i, Scalar time1 ) {
    if ( m_current.m_value )
    {
        if ( !Ra::Core::Math::areApproxEqual( m_current.m_value->getTimes()[i], time1 ) )
        {
            m_current.m_value->moveKeyFrame( i, time1 );
            m_current.updateKeyFrame( time1 );
            emit keyFrameMoved( i, time1 );
        }
    }
}

void Timeline::onMovingKeyFrames( size_t first, Scalar offset ) {
    if ( m_current.m_value )
    {
        auto times        = m_current.m_value->getTimes();
        const Scalar time = times[first];
        if ( offset < 0 )
        {
            for ( int i = 0; i < times.size(); ++i )
            {
                const Scalar t = times[i];
                if ( t >= time ) { m_current.m_value->moveKeyFrame( i, t + offset ); }
            }
        }
        else // go from the end to ensure we do not mess up keyframes
        {
            for ( int i = times.size() - 1; i >= 0; --i )
            {
                const Scalar t = times[i];
                if ( t >= time ) { m_current.m_value->moveKeyFrame( i, t + offset ); }
            }
        }
        m_current.updateKeyFrame( time );
        emit keyFramesMoved( first, offset );
    }
}

void Timeline::on_comboBox_attribute_currentIndexChanged( const QString& arg1 ) {
    onClearKeyFrames();
    Ra::Core::Animation::KeyFramedValueController frames;
    const QStringList names = arg1.split( "::" );
    switch ( names.size() )
    {
    case 2:
    {
        const std::string entityName = names.at( 0 ).toStdString();
        auto e_it                    = std::find_if(
            m_entityKeyFrames.begin(), m_entityKeyFrames.end(), [&entityName]( const auto& frame ) {
                return frame.first->getName() == entityName;
            } );
        if ( e_it == m_entityKeyFrames.end() )
        {
            LOG( logWARNING ) << "[Timeline] Error: Entity \"" << entityName
                              << "\"'s name is not conform.";
            return;
        }
        const std::string frameName = names.at( 1 ).toStdString();
        auto f_it =
            std::find_if( e_it->second.begin(),
                          e_it->second.end(),
                          [&frameName]( const auto& frame ) { return frame.m_name == frameName; } );
        if ( f_it == e_it->second.end() )
        {
            LOG( logWARNING ) << "[Timeline] Error: attribute \"" << arg1.toStdString()
                              << "\"'s name is not conform.";
            return;
        }
        frames = *f_it;
    }
    break;
    case 3:
    {
        const std::string compName = names.at( 1 ).toStdString();
        auto c_it                  = std::find_if(
            m_componentKeyFrames.begin(),
            m_componentKeyFrames.end(),
            [&compName]( const auto& frame ) { return frame.first->getName() == compName; } );
        if ( c_it == m_componentKeyFrames.end() )
        {
            LOG( logWARNING ) << "[Timeline] Error: Component \"" << compName
                              << "\"'s name is not conform.";
            return;
        }
        const std::string frameName = names.at( 2 ).toStdString();
        auto f_it =
            std::find_if( c_it->second.begin(),
                          c_it->second.end(),
                          [&frameName]( const auto& frame ) { return frame.m_name == frameName; } );
        if ( f_it == c_it->second.end() )
        {
            LOG( logWARNING ) << "[Timeline] Error: attribute \"" << arg1.toStdString()
                              << "\"'s name is not conform.";
            return;
        }
        frames = *f_it;
    }
    break;
    case 4:
    {
        const QStringList roName = names.at( 2 ).split( '_' );
        if ( roName.size() < 2 )
        {
            LOG( logWARNING ) << "[Timeline] Error: Component's name for attribute \""
                              << arg1.toStdString() << "\" is not conform.";
            return;
        }
        bool ok;
        const auto roIdx = roName.last().toInt( &ok );
        if ( !ok )
        {
            LOG( logWARNING ) << "[Timeline] Error: Component's name for attribute \""
                              << arg1.toStdString() << "\" is not conform.";
            return;
        }
        auto ro_it = std::find_if( m_renderObjectKeyFrames.begin(),
                                   m_renderObjectKeyFrames.end(),
                                   [&roIdx]( const auto& frame ) { return frame.first == roIdx; } );
        if ( ro_it == m_renderObjectKeyFrames.end() )
        {
            LOG( logWARNING ) << "[Timeline] Error: Component's name for attribute \""
                              << arg1.toStdString() << "\" is not conform.";
            return;
        }
        const std::string frameName = names.at( 3 ).toStdString();
        auto f_it =
            std::find_if( ro_it->second.begin(),
                          ro_it->second.end(),
                          [&frameName]( const auto& frame ) { return frame.m_name == frameName; } );
        if ( f_it == ro_it->second.end() )
        {
            LOG( logWARNING ) << "[Timeline] Error: attribute \"" << arg1.toStdString()
                              << "\"'s name is not conform.";
            return;
        }
        frames = *f_it;
    }
    break;
    default:
        LOG( logWARNING ) << "[Timeline] Error: attribute \"" << arg1.toStdString()
                          << "\"'s name is not conform.";
        return;
    }
    const auto times = frames.m_value->getTimes();
    for ( const auto& t : times )
    {
        ui->frame_selector->onAddingKeyFrame( t, false );
    }
    m_current = frames;
}

void Timeline::on_pushButton_editAttribute_clicked() {
    // TODO
}

void Timeline::on_toolButton_help_clicked() {
    static bool is_showing = false;
    if ( !is_showing )
    {
        HelpDialog* dialog = new HelpDialog( this );
        dialog->show();
        is_showing = true;
        connect( dialog, &HelpDialog::closed, [=]() { is_showing = false; } );
    }
}

} // namespace Ra::GuiBase
