#include <Core/Animation/Animation.hpp>
#include <Core/Animation/Pose/PoseOperation.hpp>
#include <algorithm>
#include <cmath>
#include <utility>

namespace Ra {
namespace Core {
namespace Animation {

struct KeyPoseComparator {
    bool operator()( const KeyPose& left, const KeyPose& right ) {
        return left.first < right.first;
    }
};

void Animation::addKeyPose( const Pose& pose, Scalar timestamp ) {
    m_keys.push_back( KeyPose( timestamp, pose ) );
}

void Animation::addKeyPose( const KeyPose& keyPose ) {
    m_keys.push_back( keyPose );
}

void Animation::clear() {
    m_keys.clear();
}

bool Animation::isEmpty() const {
    return m_keys.size() == 0;
}

void Animation::normalize() {
    if ( m_keys.size() == 0 )
        return;

    // sort the keys according to their timestamp
    sort( m_keys.begin(), m_keys.end(), KeyPoseComparator() );
}
Scalar Animation::getTime( Scalar timestamp ) const {
    if ( m_keys.empty() )
    {
        return 0;
    }
    Scalar duration = m_keys.back().first;
    // ping pong: d - abs(mod(x, 2 * d) - d)
    return duration - std::abs( std::fmod( timestamp, 2 * duration ) - duration );
}

Scalar Animation::getDuration() const
{
    return m_keys.back().first;
}

Pose Animation::getPose( Scalar timestamp ) const {
    Scalar modifiedTime = getTime( timestamp );
    if ( modifiedTime <= m_keys.front().first )
        return m_keys.front().second;

    for ( uint i = 0; i < m_keys.size() - 1; i++ )
    {
        if ( modifiedTime >= m_keys[i].first && modifiedTime <= m_keys[i + 1].first )
        {
            const KeyPose& prev = m_keys[i];
            const KeyPose& next = m_keys[i + 1];
            Scalar t = ( modifiedTime - prev.first ) / ( next.first - prev.first );
            return Ra::Core::Animation::interpolatePoses( m_keys[i].second, m_keys[i + 1].second,
                                                          t );
        }
    }

    return m_keys.back().second;
}

} // namespace Animation
} // namespace Core
} // namespace Ra
