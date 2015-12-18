#include <Core/Animation/Handle/Skeleton.hpp>
#include <stack>

namespace Ra {
namespace Core {
namespace Animation {

/// CONSTRUCTOR
Skeleton::Skeleton() : PointCloud(), m_graph(), m_modelSpace() { }

Skeleton::Skeleton( const uint n ) : PointCloud( n ), m_graph( n ), m_modelSpace( n ) { }

/// DESTRUCTOR
Skeleton::~Skeleton() { }

/// BONE NODE
int Skeleton::addBone( const int parent, const Transform& T, const SpaceType MODE, const Label label ) {
    switch ( MODE ) {
    case SpaceType::LOCAL: {
        m_pose.push_back( T );
        if( parent == -1 ) {
            m_modelSpace.push_back( T );
        } else {
            m_modelSpace.push_back( T * m_modelSpace[parent] );
        }
    } break;
    case SpaceType::MODEL: {
        m_modelSpace.push_back( T );
        if( parent == -1 ) {
            m_pose.push_back( T );
        } else {
            m_pose.push_back( m_modelSpace[parent].inverse() * T );
        }
    } break;
    default:
        return -1;
    }
    m_label.push_back( label );
    m_graph.addNode( parent );
    return ( size() - 1 );
}

/// SIZE
void Skeleton::clear() {
    m_pose.clear();
    m_graph.clear();
    m_modelSpace.clear();
}

/// SPACE INTERFACE
const Pose& Skeleton::getPose( const SpaceType MODE ) const {
    switch( MODE ) {
    case SpaceType::LOCAL: {
        return m_pose;
    } break;
    case SpaceType::MODEL: {
        return m_modelSpace;
    } break;
    default: {
        CORE_ASSERT( false, "Should not get here.");
        return m_pose;
    }
    }
}

void Skeleton::setPose( const Pose& pose, const SpaceType MODE ) {
    CORE_ASSERT( ( size() == pose.size() ), "Size mismatching" );
    switch( MODE ) {
    case SpaceType::LOCAL: {
        m_pose = pose;
        m_modelSpace.resize( m_pose.size() );
        for( uint i = 0; i < m_graph.size(); ++i ) {
            if( m_graph.isRoot( i ) ) {
                m_modelSpace[i] = m_pose[i];
            }
            for( const auto& child : m_graph.m_child[i] ) {
                m_modelSpace[child] = m_modelSpace[i] * m_pose[child];
            }
        }
    } break;
    case SpaceType::MODEL: {
        m_modelSpace = pose;
        m_pose.resize( m_modelSpace.size() );
        for( uint i = 0; i < m_graph.size(); ++i ) {
            if( m_graph.isRoot( i ) ) {
                m_pose[i] = m_modelSpace[i];
            }
            for( const auto& child : m_graph.m_child[i] ) {
                m_pose[child] = m_modelSpace[i].inverse() * m_modelSpace[child];
            }
        }
    } break;
    default: {
        CORE_ASSERT( false, "Should not get here");
    }
    }
}
const Transform& Skeleton::getTransform( const uint i, const SpaceType MODE ) const {
    CORE_ASSERT( ( i < size() ), "Index i out of bounds");
    switch( MODE ) {
        case SpaceType::LOCAL: {
            return m_pose[i];
        } break;
        case SpaceType::MODEL: {
            return m_modelSpace[i];
        } break;
        default: {
            CORE_ASSERT(false, "Should not get here");
            return m_pose[i];
        }
    }
}

void Skeleton::setTransform( const uint i, const Transform& T, const SpaceType MODE ) {
    CORE_ASSERT( ( i < size() ), "Index i out of bounds");
    switch( MODE ) {
    case SpaceType::LOCAL: {
        m_pose[i] = T;
        // Compute the model space pose
        if( m_graph.isRoot( i ) ) {
            m_modelSpace[i] = m_pose[i];
        } else {
            m_modelSpace[i] = m_modelSpace[m_graph.m_parent[i]] * T;
        }
        if( !m_graph.isLeaf( i ) ) {
            std::stack<uint> stack;
            stack.push( i );
            while( !stack.empty()) {
                uint parent = stack.top();
                stack.pop();
                for( const auto& child : m_graph.m_child[parent] ) {
                    m_modelSpace[child] = m_modelSpace[parent] * m_pose[child];
                    stack.push( child );
                }
            }
        }
    } break;
    case SpaceType::MODEL: {
        m_modelSpace[i] = T;
        // Compute the local space pose
        if( m_graph.isRoot( i ) ) {
            m_pose[i] = m_modelSpace[i];
        } else {
            m_pose[i] = m_modelSpace[m_graph.m_parent[i]].inverse() * T;
        }
        if( !m_graph.isLeaf( i ) ) {
            std::stack<uint> stack;
            stack.push( i );
            while( !stack.empty()) {
                uint parent = stack.top();
                stack.pop();
                for( const auto& child : m_graph.m_child[parent] ) {
                    m_pose[child] = m_modelSpace[parent].inverse() * m_modelSpace[child];
                    stack.push( child );
                }
            }
        }
    } break;
    default: {
        CORE_ASSERT( false, "Should not get there");
    }
    }
}


void Skeleton::getBonePoints( const uint i, Vector3& startOut, Vector3& endOut) const
{
    // Check bone index is valid
    CORE_ASSERT( i < m_modelSpace.size(), "invalid bone index");

    startOut = m_modelSpace[i].translation();
    // A leaf bone has length 0
    if (m_graph.isLeaf(i))
    {
        endOut = startOut;
    }
    else
    {
        const auto& children = m_graph.m_child[i];
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
