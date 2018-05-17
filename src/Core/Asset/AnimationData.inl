#include <Core/Asset/AnimationData.hpp>

#include <Core/Utils/Log.hpp>

namespace Ra {
namespace Core {
namespace Asset {

/// NAME
inline void AnimationData::setName( const std::string& name ) {
    m_name = name;
}

/// TIME
inline const AnimationTime& AnimationData::getTime() const {
    return m_time;
}

inline void AnimationData::setTime( const AnimationTime& time ) {
    m_time = time;
}

inline Time AnimationData::getTimeStep() const {
    return m_dt;
}

inline void AnimationData::setTimeStep( const Time& delta ) {
    m_dt = delta;
}

/// KEY FRAME
inline uint AnimationData::getFramesSize() const {
    return m_keyFrame.size();
}

inline std::vector<HandleAnimation> AnimationData::getFrames() const {
    return m_keyFrame;
}

inline void AnimationData::setFrames( const std::vector<HandleAnimation>& frameList ) {
    const uint size = frameList.size();
    m_keyFrame.resize( size );
#pragma omp parallel for
    for ( int i = 0; i < int( size ); ++i )
    {
        m_keyFrame[i] = frameList[i];
    }
}

/// DEBUG
inline void AnimationData::displayInfo() const {
    LOG( Utils::logDEBUG ) << "======== ANIMATION INFO ========";
    LOG( Utils::logDEBUG ) << " Name              : " << m_name;
    LOG( Utils::logDEBUG ) << " Start Time        : " << m_time.getStart();
    LOG( Utils::logDEBUG ) << " End   Time        : " << m_time.getEnd();
    LOG( Utils::logDEBUG ) << " Time Step         : " << m_dt;
    LOG( Utils::logDEBUG ) << " Animated Object # : " << m_keyFrame.size();
}

} // namespace Asset
} // namespace Core
} // namespace Ra
