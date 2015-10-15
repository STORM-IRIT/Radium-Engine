#include <Core/Animation/Animation.hpp>
#include <algorithm>

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
    for (KeyPose keyPose : m_keys)
    {
        if (keyPose.first > timestamp)
            return keyPose.second;
    }
    
    return m_keys.back().second;
}

}
}
}
