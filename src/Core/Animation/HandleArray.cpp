#include <Core/Animation/HandleArray.hpp>

namespace Ra {
namespace Core {
namespace Animation {

HandleArray::HandleArray() : m_pose(), m_name( "" ), m_label() {}

HandleArray::HandleArray( const uint n ) :
    m_pose( n, Transform::Identity() ),
    m_name( "" ),
    m_label( n, "" ) {}

HandleArray::~HandleArray() {}

void HandleArray::clear() {
    m_pose.clear();
}

const Pose& HandleArray::getPose( const SpaceType /*MODE*/ ) const {
    return m_pose;
}

void HandleArray::setPose( const Pose& pose, const SpaceType /*MODE*/ ) {
    m_pose = pose;
}

const Transform& HandleArray::getTransform( const uint i, const SpaceType /*MODE*/ ) const {
    CORE_ASSERT( ( i < size() ), "Index i out of bound" );
    return m_pose[i];
}

void HandleArray::setTransform( const uint i, const Transform& T, const SpaceType /*MODE*/ ) {
    CORE_ASSERT( ( i < size() ), "Index i out of bound" );
    m_pose[i] = T;
}

} // namespace Animation
} // Namespace Core
} // Namespace Ra
