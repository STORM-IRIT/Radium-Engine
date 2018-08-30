
#ifndef RADIUMENGINE_BVH_HPP
#define RADIUMENGINE_BVH_HPP

#include <Core/RaCore.hpp>

#include <Core/Math/Frustum.hpp>
#include <Core/Mesh/TriangleMesh.hpp>

#include <memory>
#include <vector>

namespace Ra {
namespace Core {

/// This class stores a 3-dimensional hierarchy of meshes of arbitrary type.
/// Built on a binary tree.
template <typename T>
class BVH {
    /// A Node stores its children  nodes in the BVH as well as the AABB of their content.
    class Node {
      public:
        inline Node( const std::shared_ptr<T>& t );

        inline Node( const std::shared_ptr<Node>& l, const std::shared_ptr<Node>& r );

        /// Return the left child in the BVH.
        inline std::shared_ptr<Node> getLeftChild() const;

        /// Return the right child in the BVH.
        inline std::shared_ptr<Node> getRightChild() const;

        /// Return the AABB of the content.
        inline Aabb getAabb() const { return m_aabb; }

        /// Return the node's content, available only for leaves.
        inline std::shared_ptr<T> getData() { return m_data; }

        /// Return true if the Node is a leaf in the BVH.
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
    RA_CORE_ALIGNED_NEW

    inline BVH();

    inline BVH( const BVH& other ) = default;

    inline BVH& operator=( const BVH& other ) = default;

    /// Add a new leaf node containing \p t.
    inline void insertLeaf( const std::shared_ptr<T>& t );

    // TODO removeLeaf()

    /// Remove all content, leaving the BVH empty.
    inline void clear();

    /// Call buildBottomUpSlow() if needed.
    inline void update();

    /// Rebuild the entire BVH from the leaves to the root.
    inline void buildBottomUpSlow();

    // TODO void buildBottomUpFast();

    // TODO void buildTopDown();

    /// Output the list of meshes for which mesh \f$ \cap \f$ frustum \f$ \neq \emptyset \f$.
    void getInFrustumSlow( std::vector<std::shared_ptr<T>>& objects, const Frustum& frustum ) const;

  protected:
    /// The list of tree leaves.
    std::vector<NodePtr> m_leaves;

    /// The tree root.
    NodePtr m_root;

    /// The tree AABB.
    Aabb m_root_aabb;

    /// Whether the tree is up-to-date.
    bool m_upToDate;
};

} // namespace Core
} // namespace Ra

#include <Core/TreeStructures/BVH.inl>

#endif // RADIUMENGINE_BVH_HPP
