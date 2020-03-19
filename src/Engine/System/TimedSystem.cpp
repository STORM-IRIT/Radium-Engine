#include <Engine/System/TimedSystem.hpp>

#include <Core/Utils/Log.hpp>
#include <Engine/FrameInfo.hpp>

#include <algorithm>
#include <cmath>

using namespace Ra::Core::Utils;

namespace Ra::Engine {

TimeSystem::TimeSystem( Scalar dt, Scalar endTime ) :
    BaseCouplingSystem<AbstractTimedSystem>(),
    m_dt( dt ),
    m_endTime( endTime ) {}

void TimeSystem::generateTasks( Ra::Core::TaskQueue* taskQueue,
                                const Ra::Engine::FrameInfo& frameInfo ) {
    const bool playFrame = m_play || m_step;

    if ( playFrame )
    {
        ++m_frame;
        updateTime();
        m_lockFrame = true;
        goTo( m_time );
        m_lockFrame = false;
    }

    // deal with AbstractTimedSystems
    BaseCouplingSystem<AbstractTimedSystem>::generateTasks( taskQueue, frameInfo );

    m_step = false;
}

void TimeSystem::cacheFrame( const std::string& dir ) const {
    cacheFrame( dir, getFrame() );
}

void TimeSystem::cacheFrame( const std::string& dir, uint frameID ) const {
    dispatch( [&dir, frameID]( const auto& s ) { s->cacheFrame( dir, frameID ); } );
}

bool TimeSystem::restoreFrame( const std::string& dir, uint frameID ) {
    static bool restoringCurrent = false;
    if ( !restoringCurrent )
    {
        // first save current, in case restoration fails.
        cacheFrame( dir, m_frame );
    }

    // restore AbstractTimedSystems
    bool success = true;
    success &= conditionnaldispatch(
        [&dir, frameID]( const auto& s ) { return s->restoreFrame( dir, frameID ); } );

    // if one failed, restore current frame
    if ( !success && !restoringCurrent )
    {
        restoringCurrent = true;
        restoreFrame( dir, m_frame );
        restoringCurrent = false;
        return false;
    }

    if ( success ) { m_frame = frameID; }
    else
    {
        LOG( logWARNING ) << "TimeSystem error: failed to restore frame " << frameID
                          << " but also failed to restore current frame " << m_frame
                          << ". Time-related systems might mysbehave.";
    }
    return success;
}

void TimeSystem::goTo( Scalar t ) {
    m_time = t;
    if ( !m_lockFrame ) { m_frame = uint( std::ceil( t / m_dt ) ); }
    // todo (v2): deal with TimeComponents instead of systems
    dispatch( [&t]( const auto& s ) { s->goTo( t ); } );
}

void TimeSystem::play( bool isPlaying ) {
    m_play = isPlaying;
}

void TimeSystem::step() {
    m_step = true;
}

void TimeSystem::reset() {
    m_play = false;
    m_step = false;
    goTo( uint( 0 ) );
}

void TimeSystem::setStartTime( Scalar t ) {
    m_startTime = std::max( t, 0_ra );
}

Scalar TimeSystem::getStartTime() const {
    return m_startTime;
}

void TimeSystem::setEndTime( Scalar t ) {
    m_endTime = t;
}

Scalar TimeSystem::getEndTime() const {
    return m_endTime;
}

void TimeSystem::setPingPongMode( bool mode ) {
    m_pingPongMode = mode;
}

Scalar TimeSystem::getTime() const {
    return m_time;
}

uint TimeSystem::getFrame() const {
    return uint( std::ceil( m_time / m_dt ) );
}

void TimeSystem::updateTime() {
    uint startFrame = uint( std::ceil( m_startTime / m_dt ) );
    uint endFrame   = uint( std::ceil( m_endTime / m_dt ) );
    if ( m_frame < startFrame || ( !m_pingPongMode && m_endTime > 0 && m_frame > endFrame ) ||
         ( m_pingPongMode && m_endTime > 0 && m_frame > 2 * endFrame - startFrame ) )
    {
        // restart
        m_frame = startFrame;
    }
    if ( m_pingPongMode && m_endTime > 0 && m_frame > endFrame )
    {
        // ping pong
        m_time = ( 2 * endFrame - m_frame ) * m_dt;
    }
    else
    { m_time = m_frame * m_dt; }
}

} // namespace Ra::Engine
