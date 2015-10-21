#include <Core/Animation/Animation.hpp>
#include <algorithm>
#include <Core/Animation/Pose/PoseOperation.hpp>
#include <iostream>
#include <cmath>

namespace Ra {
namespace Core {
namespace Animation {

struct KeyPoseComparator
{
    bool operator()(const KeyPose& left, const KeyPose &right)
    {
        return left.first < right.first;
    }
};

void Animation::addKeyPose(const Pose& pose, Scalar timestamp)
{
    m_keys.push_back(KeyPose(timestamp, pose));
}

void Animation::addKeyPose(const KeyPose& keyPose)
{
    m_keys.push_back(keyPose);
}

void Animation::clear()
{
    m_keys.clear();
}

void Animation::normalize()
{
    if (m_keys.size() == 0)
        return;
    
    // sort the keys according to their timestamp
    sort(m_keys.begin(), m_keys.end(), KeyPoseComparator());
    
}

Pose Animation::getPose(Scalar timestamp) const
{
    Scalar duration = m_keys.back().first;
    
    // ping pong: d - abs(mod(x, 2 * d) - d)
    Scalar modifiedTime = duration - std::abs(fmod(timestamp, 2 * duration) - duration);
 
    if (modifiedTime <= m_keys.front().first)
        return m_keys.front().second;
    
    for (int i = 0; i < m_keys.size() - 1; i++)
    {
        if (modifiedTime >= m_keys[i].first && modifiedTime <= m_keys[i + 1].first)
        {
            const KeyPose& prev = m_keys[i];
            const KeyPose& next = m_keys[i + 1];
            Scalar t = (modifiedTime - prev.first) / (next.first - prev.first);
            return Ra::Core::Animation::interpolatePoses(m_keys[i].second, m_keys[i + 1].second, t);
        }
    }
    
    return m_keys.back().second;
}

}
}
}
