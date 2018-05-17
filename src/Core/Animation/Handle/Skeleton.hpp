#ifndef SKELETON_HANDLE_H
#define SKELETON_HANDLE_H

#include <Core/Animation/Handle/Handle.hpp>
#include <Core/Utils/Graph/AdjacencyList.hpp>

namespace Ra {
namespace Core {
namespace Animation {

/**
 * The Skeleton handle class.
 *
 * A skeleton handle is a set of joint transforms with an associated joint hierarchy,
 * represented by a graph ( adjacency list ).
 *
 * Skeleton bones represent a couple of joints: the proximal joint and the distal joint,
 * the former begin the parent of the latter in the hierarchy.
 * For animation purposes, a bone transform refers to the proximal joint's tranform.
 */
class RA_CORE_API Skeleton : public Handle {
  public:
    Skeleton();
    Skeleton( const uint n );
    Skeleton( const Skeleton& skeleton ) = default;

    ~Skeleton();

    inline uint size() const override { return m_graph.size(); }
    void clear() override;
    const Pose& getPose( const SpaceType MODE ) const override;
    void setPose( const Pose& pose, const SpaceType MODE ) override;
    const Transform& getTransform( const uint i, const SpaceType MODE ) const override;
    void setTransform( const uint i, const Transform& T, const SpaceType MODE ) override;

    /**
     * Add a new joint transform to the skeleton.
     * @param parent the index of the joint's parent in the hierarchy
     * @param T      the joint transform associated to the new bone
     * @param MODE   SpaceType of T (either SpaceType::LOCAL or SpaceType::GLOBAL)
     * @param label  the name for the new joint
     * @return       the index of the new joint
     */
    int addBone( const int parent = -1, const Transform& T = Transform::Identity(),
                 const SpaceType MODE = SpaceType::LOCAL, const Label label = "" );

    /**
     * Get the i-th bone endpoints.
     * @param i             the bone index
     * @param[out] startOut the bone's start point
     * @param[ou]  endOut   the bone's end point
     */
    void getBonePoints( uint i, Vector3& startOut, Vector3& endOut ) const;

    /**
     * Project point \p pos onto the bone with index \p boneIdx.
     */
    Ra::Core::Vector3 projectOnBone( int boneIdx, const Ra::Core::Vector3& pos ) const;

    /**
     * Stream insertion operator.
     */
    friend std::ostream& operator<<( std::ostream& os, const Skeleton& skeleton );

    /**
     * Joint hierarchy.
     */
    Ra::Core::Graph::AdjacencyList m_graph;

  protected:
    /**
     * Skeleton pose in MODEL space.
     */
    ModelPose m_modelSpace;
};

} // namespace Animation
} // Namespace Core
} // Namespace Ra

#endif // SKELETON_HANDLE_H
