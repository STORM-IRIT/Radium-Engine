#include <Core/Animation/Handle.hpp>

namespace Ra {
namespace Core {
namespace Animation {

/// CONSTRUCTOR
Handle::Handle() : m_pose(), m_name( "" ), m_label() {}

Handle::Handle( const uint n ) :
    m_pose( n, Math::Transform::Identity() ),
    m_name( "" ),
    m_label( n, "" ) {}

/// DESTRUCTOR
Handle::~Handle() {}

/// SIZE
void Handle::clear() {
    m_pose.clear();
}

/// SPACE INTERFACE
const Pose& Handle::getPose( const SpaceType MODE ) const {
    return m_pose;
}

void Handle::setPose( const Pose& pose, const SpaceType MODE ) {
    m_pose = pose;
}

const Math::Transform& Handle::getTransform( const uint i, const SpaceType MODE ) const {
    CORE_ASSERT( ( i < size() ), "Index i out of bound" );
    return m_pose.at( i );
}

void Handle::setTransform( const uint i, const Math::Transform& T, const SpaceType MODE ) {
    CORE_ASSERT( ( i < size() ), "Index i out of bound" );
    m_pose[i] = T;
}

} // namespace Animation
} // Namespace Core
} // Namespace Ra
