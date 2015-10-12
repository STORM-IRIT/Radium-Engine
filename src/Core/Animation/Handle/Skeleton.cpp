#include <Core/Animation/Handle/Skeleton.hpp>

namespace Ra {
namespace Core {
namespace Animation {

/// CONSTRUCTOR
Skeleton::Skeleton() : PointCloud(), m_hier(), m_modelSpace() { }

Skeleton::Skeleton( const uint n ) : PointCloud( n ), m_hier( n ), m_modelSpace( n ) { }

Skeleton::Skeleton( const Skeleton& skeleton ) : PointCloud( skeleton ), m_hier( skeleton.m_hier ), m_modelSpace( skeleton.m_modelSpace ) { }

/// DESTRUCTOR
Skeleton::~Skeleton() { }

/// SIZE
void Skeleton::clear() {
    m_pose.clear();
    m_hier.clear();
    m_modelSpace.clear();
}



/// SPACE INTERFACE
Pose Skeleton::getPose( const SpaceType MODE ) const {
    switch( MODE ) {
    case SpaceType::LOCAL: {
        return m_pose;
    } break;
    case SpaceType::MODEL: {
        return m_modelSpace;
    } break;
    default: {
        CORE_ASSERT( false, "Should not ");
        return Pose();
    }
    }
}



void Skeleton::setPose( const Pose& pose, const SpaceType MODE ) {
    CORE_ASSERT( ( size() == pose.size() ), "Size mismatching" );
    switch( MODE ) {
    case SpaceType::LOCAL: {
        m_pose = pose;
        m_modelSpace.resize( m_pose.size() );
        for( uint i = 0; i < m_hier.size(); ++i ) {
            if( m_hier.isRoot( i ) ) {
                m_modelSpace[i] = m_pose[i];
            }
            for( auto child : m_hier.m_child[i] ) {
                m_modelSpace[child] = m_modelSpace[i] * m_pose[child];
            }
        }
    } break;
    case SpaceType::MODEL: {
        m_modelSpace = pose;
        m_pose.resize( m_modelSpace.size() );
        for( uint i = 0; i < m_hier.size(); ++i ) {
            if( m_hier.isRoot( i ) ) {
                m_pose[i] = m_modelSpace[i];
            }
            for( auto child : m_hier.m_child[i] ) {
                m_pose[child] = m_modelSpace[i].inverse() * m_modelSpace[child];
            }
        }
    } break;
    default: {
        CORE_ASSERT( false, "WHAT THE HELL JUST HAPPENED????");
    }
    }
}
Transform Skeleton::getTransform( const uint i, const SpaceType MODE ) const {
    CORE_ASSERT( ( i < size() ), "Index i out of bounds");
    switch( MODE ) {
        case SpaceType::LOCAL:
        {
            return m_pose.at( i );
        } break;
        case SpaceType::MODEL: {
            return m_modelSpace.at( i );
        } break;
        default: {
            CORE_ASSERT(false, "Should not get here");
            return Transform::Identity();
        }
    }
}

void Skeleton::setTransform( const uint i, const Transform& T, const SpaceType MODE ) {
    CORE_ASSERT( ( i < size() ), "Index i out of bounds");
    switch( MODE ) {
    case SpaceType::LOCAL: {
        m_pose[i] = T;
        // Compute the model space pose
        if( m_hier.isRoot( i ) ) {
            m_modelSpace[i] = m_pose[i];
        } else {
            m_modelSpace[i] = m_modelSpace[m_hier.m_parent[i]] * T;
        }
        if( !m_hier.isLeaf( i ) ) {
            Graph::ParentList stack;
            stack.push_back( i );
            for( auto parent : stack ) {
                for( auto child : m_hier.m_child[parent] ) {
                    m_modelSpace[child] = m_modelSpace[parent] * m_pose[child];
                    if( !m_hier.isLeaf( child ) ) {
                        stack.push_back( child );
                    }
                }
            }
        }
    } break;
    case SpaceType::MODEL: {
        m_modelSpace[i] = T;
        // Compute the local space pose
        if( m_hier.isRoot( i ) ) {
            m_pose[i] = m_modelSpace[i];
        } else {
            m_pose[i] = m_modelSpace[m_hier.m_parent[i]].inverse() * T;
        }
        if( !m_hier.isLeaf( i ) ) {
            Graph::ParentList stack;
            stack.push_back( i );
            for( auto parent : stack ) {
                for( auto child : m_hier.m_child[parent] ) {
                    m_pose[child] = m_modelSpace[parent].inverse() * m_modelSpace[child];
                    if( !m_hier.isLeaf( child ) ) {
                        stack.push_back( child );
                    }
                }
            }
        }

    } break;
    default: {
        CORE_ASSERT( false, "WHAT THE HELL JUST HAPPENED????");
    }
    }
}


void Skeleton::getBonePoints( const uint i, Vector3& startOut, Vector3& endOut) const
{
    // Check bone index is valid
    CORE_ASSERT( i < m_modelSpace.size(), "invalid bone index");

    startOut = m_modelSpace[i].translation();
    // A leaf bone has length 0
    if (m_hier.isLeaf(i))
    {
        endOut = startOut;
    }
    else
    {
        const auto& children = m_hier.m_child[i];
        CORE_ASSERT( children.size() > 0, "non-leaf bone has no children.");
        // End point is the average of chidren start points.
        endOut = Vector3::Zero();
        for (auto child : children)
        {
            endOut += m_modelSpace[child].translation();
        }
        endOut *= (1.f / children.size());
    }
}

} // namespace Animation
} // Namespace Core
} // Namespace Ra
