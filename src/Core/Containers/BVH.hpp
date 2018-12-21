#ifndef RADIUMENGINE_BVH_HPP
#define RADIUMENGINE_BVH_HPP

#include <Core/RaCore.hpp>
#include <Eigen/Geometry>

#include <Core/Geometry/Frustum.hpp>

#include <memory>
#include <vector>

namespace Ra {
namespace Core {
namespace Containers {
/*!
 * \brief Stores a 3-dimensional hierarchy of meshes of arbitrary type.
 *
 * This class is marked as deprecated as it is not tested nor used.
 * \FIXME Confirm class status: add tests + use cases *or* remove
 *
 * \note Radium might soon have a generic type for geometrical objects.
 * This would allow to remove the template parameter of this class.
 *
 * \note We might consider using this instead:
 * https://eigen.tuxfamily.org/dox/unsupported/group__BVH__Module.html
 */
template <typename T>
class [[deprecated( "Neither used nor tested" )]] BVH {
    using Aabb = Eigen::AlignedBox<Scalar, 3>;
    class Node {
      public:
        inline Node( const std::shared_ptr<T>& t );

        inline Node( const std::shared_ptr<Node>& l, const std::shared_ptr<Node>& r );

        inline std::shared_ptr<Node> getLeftChild() const;
        inline std::shared_ptr<Node> getRightChild() const;

        inline const Aabb& getAabb() const { return m_aabb; }

        inline std::shared_ptr<T> getData() { return m_data; }

        inline bool isFinal() const { return m_children.empty(); }

      protected:
        Aabb m_aabb;
        std::vector<std::shared_ptr<Node>> m_children;

        std::shared_ptr<T> m_data;
    };

  public:
    // public types and constants.
    using NodePtr = std::shared_ptr<Node>;

    using Nodep = Node*;

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    inline BVH();

    inline BVH( const BVH& other ) = default;
    inline BVH& operator=( const BVH& other ) = default;

    inline void insertLeaf( const std::shared_ptr<T>& t );

    // TODO removeLeaf()

    inline void clear();

    inline void update();

    inline void buildBottomUpSlow();

    // TODO void buildBottomUpFast();

    // TODO void buildTopDown();

    void getInFrustumSlow( std::vector<std::shared_ptr<T>> & objects,
                           const Geometry::Frustum& frustum ) const;

  protected:
    std::vector<NodePtr> m_leaves;
    NodePtr m_root;
    Aabb m_root_aabb;

    bool m_upToDate;
};
} // namespace Containers
} // namespace Core
} // namespace Ra

#include <Core/Containers/BVH.inl>

#endif // RADIUMENGINE_BVH_HPP
