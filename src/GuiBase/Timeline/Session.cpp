#include <GuiBase/Timeline/Configurations.h>
#include <GuiBase/Timeline/Session.h>

#include <QTimer>
#include <set>

#include <GuiBase/Timeline/qdoublespinboxsmart.h>
#include <GuiBase/Timeline/qframeselector.h>
#include <GuiBase/Timeline/qspinboxsmart.h>
#include <GuiBase/Timeline/qtoolbuttonplaypause.h>
#include <GuiBase/Timeline/qwidgetruler.h>

namespace Ra::GuiBase {

Session::Session( QObject* parent ) : QObject( parent ) {
    m_saveDelay = new QTimer( this );
    connect( m_saveDelay, &QTimer::timeout, this, &Session::envSaved );
    m_saveDelay->setSingleShot( true );
}

Session::~Session() {
    onClearSession();
    delete m_saveDelay;
}

// timeline changed, save timeline and anim environment
void Session::onChangeEnv() {
    m_saveDelay->start( TIMELINE_DELAY_AUTO_SAVE );
}

void Session::onClearSession() {
    while ( !m_undo.empty() )
    {
        emit renderDeleted( m_undo.back().m_anim );
        m_undo.pop_back();
    }
    while ( !m_redoHeap.empty() )
    {
        emit renderDeleted( m_redoHeap.top().m_anim );
        m_redoHeap.pop();
    }

    m_size = 0;

    onChangeEnv();
}

void Session::onUndo() {
    if ( m_undo.empty() ) { return; }

    if ( m_undo.size() > 1 )
    {
        m_redoHeap.push( m_undo.back() );
        m_undo.pop_back();

        setEnv( m_undo.back() );
    }
}

void Session::onRedo() {
    if ( m_redoHeap.empty() ) { return; }

    m_undo.emplace_back( m_redoHeap.top() );
    m_redoHeap.pop();

    setEnv( m_undo.back() );
}

void Session::onSaveRendering( void* anim, size_t bytes ) {

    while ( !m_redoHeap.empty() )
    {
        emit renderDeleted( m_redoHeap.top().m_anim );
        m_size -= m_redoHeap.top().m_bytes;
        m_redoHeap.pop();
    }

    m_undo.emplace_back( Env{*m_start, *m_end, *m_cursor, *m_duration, *m_keyPoses, anim, bytes} );
    m_size += bytes;

    while ( m_size > TIMELINE_BUFFER_SESSION_MAX_SIZE )
    {
        emit renderDeleted( m_undo.front().m_anim );
        m_size -= m_undo.front().m_bytes;
        m_undo.pop_front();
    }
}

void Session::setEnv( Env env ) {
    *m_start = env.m_start;
    m_selector->updateStartSpin();

    *m_end = env.m_end;
    m_selector->updateEndSpin();

    *m_keyPoses = env.m_keyPoses;
    m_nbKeyPosesSpin->setValue( static_cast<int>( m_keyPoses->size() ) );

    m_selector->updateCursorSpin();

    *m_duration = env.m_duration;
    m_selector->updateDurationSpin();

    m_selector->redrawPlayZone();
    m_ruler->onDrawRuler( m_ruler->width() );

    emit rendered( env.m_anim );
}

void Session::setKeyPoses( std::set<double>* value ) {
    m_keyPoses = value;
}

void Session::setDuration( double* value ) {
    m_duration = value;
}

void Session::setCursor( double* value ) {
    m_cursor = value;
}

void Session::setEnd( double* value ) {
    m_end = value;
}

void Session::setStart( double* value ) {
    m_start = value;
}

void Session::setDurationSpin( QDoubleSpinBoxSmart* value ) {
    m_durationSpin = value;
}

void Session::setCursorSpin( QDoubleSpinBoxSmart* value ) {
    m_cursorSpin = value;
}

void Session::setEndSpin( QDoubleSpinBoxSmart* value ) {
    m_endSpin = value;
}

void Session::setStartSpin( QDoubleSpinBoxSmart* value ) {
    m_startSpin = value;
}

void Session::setNbKeyPosesSpin( QSpinBoxSmart* value ) {
    m_nbKeyPosesSpin = value;
}

void Session::setSelector( QFrameSelector* value ) {
    m_selector = value;
}

void Session::setRuler( QWidgetRuler* value ) {
    m_ruler = value;
}

void Session::setPlayButton( QToolButtonPlayPause* value ) {
    m_playButton = value;
}

} // namespace Ra::GuiBase
