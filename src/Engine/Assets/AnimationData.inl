#include <Engine/Assets/AnimationData.hpp>

#include <Core/Log/Log.hpp>

namespace Ra {
namespace Asset {

/// NAME
inline std::string AnimationData::getName() const {
    return m_name;
}

/// TIME
inline AnimationTime AnimationData::getTime() const {
    return m_time;
}

inline Time AnimationData::getTimeStep() const {
    return m_dt;
}

/// KEY FRAME
inline uint AnimationData::getFramesSize() const {
    return m_keyFrame.size();
}

inline std::vector< HandleAnimation > AnimationData::getFrames() const {
    return m_keyFrame;
}

/// DEBUG
inline void AnimationData::displayInfo() const {
    LOG( logDEBUG ) << "======== ANIMATION INFO ========";
    LOG( logDEBUG ) << " Name              : " << m_name;
    LOG( logDEBUG ) << " Start Time        : " << m_time.getStart();
    LOG( logDEBUG ) << " End   Time        : " << m_time.getEnd();
    LOG( logDEBUG ) << " Time Step         : " << m_dt;
    LOG( logDEBUG ) << " Animated Object # : " << m_keyFrame.size();
}

/// NAME
inline void AnimationData::setName( const std::string& name ) {
    m_name = name;
}

/// TIME
inline void AnimationData::setTime( const AnimationTime& time ) {
    m_time = time;
}

inline void AnimationData::setTimeStep( const Time& delta ) {
    m_dt = delta;
}

/// KEYFRAME
inline void AnimationData::setFrames( const std::vector< HandleAnimation >& frameList ) {
    const uint size = frameList.size();
    m_keyFrame.resize( size );
#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        m_keyFrame[i] = frameList[i];
    }
}


} // namespace Asset
} // namespace Ra
