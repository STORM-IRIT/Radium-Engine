#ifndef RADIUMENGINE_ANIMATION_DATA_HPP
#define RADIUMENGINE_ANIMATION_DATA_HPP

#include <string>
#include <vector>

#include <Core/Log/Log.hpp>

#include <Engine/Assets/AssimpAnimationDataLoader.hpp>
#include <Engine/Assets/KeyFrame/AnimationTime.hpp>
#include <Engine/Assets/KeyFrame/KeyTransform.hpp>

#include <Engine/Assets/FileData.hpp>

// FIXME(Charly): Needs to be fixed to be compiled
#ifdef DEBUG_LOAD_ANIMATION

namespace Ra {
namespace Asset {

struct HandleAnimation {
    HandleAnimation( const std::string& name = "" ) :
        m_name( name ),
        m_anim() { }

    std::string   m_name;
    KeyTransform  m_anim;
};

class AnimationData {
public:
    /// FRIEND
    friend class AssimpAnimationDataLoader;

    /// CONSTRUCTOR
    AnimationData( const std::string& name = "" ) :
        m_name( name ),
        m_time(),
        m_dt( 0.0 ),
        m_keyFrame() { }

    /// DESTRUCTOR
    ~AnimationData() { }

    /// NAME
    inline std::string getName() const {
        return m_name;
    }

    /// TIME
    inline AnimationTime getTime() const {
        return m_time;
    }

    inline Time getTimeStep() const {
        return m_dt;
    }

    /// KEY FRAME
    inline uint getFramesSize() const {
        return m_keyFrame.size();
    }

    inline std::vector< HandleAnimation > getFrames() const {
        return m_keyFrame;
    }

    /// DEBUG
    inline void displayInfo() const {
        LOG( logDEBUG ) << "======== ANIMATION INFO ========";
        LOG( logDEBUG ) << " Name            : " << m_name;
        LOG( logDEBUG ) << " Start Time      : " << m_time.m_start;
        LOG( logDEBUG ) << " End   Time      : " << m_time.m_end;
        LOG( logDEBUG ) << " Time Step       : " << m_dt;
        LOG( logDEBUG ) << " KeyFrame #      : " << m_keyFrame.size();
    }

protected:
    /// NAME
    inline void setName( const std::string& name ) {
        m_name = name;
    }

    /// TIME
    inline void setTime( const AnimationTime& time ) {
        m_time = time;
    }

    inline void setTimeStep( const Time& delta ) {
        m_dt = delta;
    }

    /// KEYFRAME
    inline void setFrames( const std::vector< HandleAnimation >& frameList ) {
        const uint size = frameList.size();
        m_keyFrame.resize( size );
#pragma omp parallel for
        for( uint i = 0; i < size; ++i ) {
            m_keyFrame[i] = frameList[i];
        }
    }

protected:
    /// VARIABLE
    std::string                    m_name;
    AnimationTime                  m_time;
    Time                           m_dt;
    std::vector< HandleAnimation > m_keyFrame;
};



} // namespace Asset
} // namespace Ra

#endif // RADIUMENGINE_ANIMATION_DATA_HPP

#endif
