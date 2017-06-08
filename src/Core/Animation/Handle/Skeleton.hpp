#ifndef SKELETON_HANDLE_H
#define SKELETON_HANDLE_H

#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Animation/Handle/PointCloud.hpp>
#include <Core/Utils/Graph/AdjacencyList.hpp>

namespace Ra {
namespace Core {
namespace Animation {

/**
* The Skeleton handle class.
*
* A skeleton handle is a set of transforms with an associated hierarchy, represented by a graph ( adjacency list ).
*/
class RA_CORE_API Skeleton : public PointCloud {
public:
    /// CONSTRUCTOR
    Skeleton();                             // Default constructor
    Skeleton( const uint n );               // Reserve the memory for n transforms
    Skeleton( const Skeleton& skeleton ) = default;// Copy constructor

    /// DESTRUCTOR
    ~Skeleton();

    /// BONE NODE
    int addBone( const int parent = -1, const Transform& T = Transform::Identity(), const SpaceType MODE = SpaceType::LOCAL, const Label label = "" );

    /// SIZE
    inline uint size() const override {
        return m_graph.size();
    }
    virtual void clear() override;  // Empty the data of the skeleton

    /// SPACE INTERFACE
    virtual const Pose&      getPose( const SpaceType MODE ) const override;                                  // Return the pose in MODE space
    virtual void             setPose( const Pose& pose, const SpaceType MODE ) override;                      // Set the MODE space pose
    virtual const Transform& getTransform( const uint i, const SpaceType MODE ) const override;               // Return the i-th transform matrix of the pose in MODE space
    virtual void             setTransform( const uint i, const Transform& T, const SpaceType MODE ) override; // Set the i-th transform of the MODE space pose

    void getBonePoints( uint i, Vector3& startOut, Vector3& endOut ) const;

    /// VARIABLE
    Graph::AdjacencyList m_graph; // The adjacency list.

protected:
    /// VARIABLE
    ModelPose m_modelSpace;
};

} // namespace Animation
} // Namespace Core
} // Namespace Ra

//#include <Core/Animation/Handle/Skeleton.inl>

#endif // SKELETON_HANDLE_H

