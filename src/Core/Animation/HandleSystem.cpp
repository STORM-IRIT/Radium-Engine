#include <Core/Animation/HandleSystem.hpp>

namespace Ra {
namespace Core {
namespace Animation {

HandleSystem::HandleSystem() : m_pose(), m_name( "" ), m_label() {}

HandleSystem::HandleSystem( const uint n ) :
    m_pose( n, Transform::Identity() ),
    m_name( "" ),
    m_label( n, "" ) {}

HandleSystem::~HandleSystem() {}

void HandleSystem::clear() {
    m_pose.clear();
}

const Pose& HandleSystem::getPose( const SpaceType /*MODE*/ ) const {
    return m_pose;
}

void HandleSystem::setPose( const Pose& pose, const SpaceType /*MODE*/ ) {
    m_pose = pose;
}

const Transform& HandleSystem::getTransform( const uint i, const SpaceType /*MODE*/ ) const {
    CORE_ASSERT( ( i < size() ), "Index i out of bound" );
    return m_pose[i];
}

void HandleSystem::setTransform( const uint i, const Transform& T, const SpaceType /*MODE*/ ) {
    CORE_ASSERT( ( i < size() ), "Index i out of bound" );
    m_pose[i] = T;
}

} // namespace Animation
} // Namespace Core
} // Namespace Ra
