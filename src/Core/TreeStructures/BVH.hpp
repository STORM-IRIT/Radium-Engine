 
#ifndef RADIUMENGINE_BVH_HPP
#define RADIUMENGINE_BVH_HPP

#include <Core/RaCore.hpp>

#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Math/Frustum.hpp>

#include <vector>
#include <memory>



namespace Ra
{
    namespace Core
    {
        /// This class stores a 3-dimensional hierarchy of meshes of arbitrary type.
        /// Built on a binary tree
        template <typename T>
        class BVH
        {
            class Node {
            public:
                inline Node( const std::shared_ptr<T>& t );

                inline Node( const std::shared_ptr<Node>& l, const std::shared_ptr<Node>& r );

                inline std::shared_ptr<Node> getLeftChild() const;
                inline std::shared_ptr<Node> getRightChild() const;

                inline Aabb getAabb() const
                {
                    return m_aabb;
                }

                inline std::shared_ptr<T> getData()
                {
                    return m_data;
                }

                inline bool isFinal() const
                {
                    return m_children.empty();
                }

            protected:
                Aabb m_aabb ;
                std::vector<std::shared_ptr<Node>> m_children ;

                std::shared_ptr<T> m_data;
            };

        public:
            // public types and constants.
            typedef std::shared_ptr<Node> NodePtr;

            typedef Node * Nodep;

        public:
            RA_CORE_ALIGNED_NEW

            inline BVH();

            inline BVH( const BVH& other ) = default;
            inline BVH& operator= ( const BVH& other ) = default;

            inline void insertLeaf(const std::shared_ptr<T>& t);

            //TODO removeLeaf()

            inline void clear();

            inline void update();

            inline void buildBottomUpSlow();

            //TODO void buildBottomUpFast();

            //TODO void buildTopDown();

            void getInFrustumSlow(std::vector<std::shared_ptr<T>> & objects, const Frustum & frustum) const;

        protected:
            std::vector<NodePtr> m_leaves;
            NodePtr m_root;
            Aabb m_root_aabb;

            bool m_upToDate;
        };
    }
}

#include <Core/TreeStructures/BVH.inl>

#endif //RADIUMENGINE_BVH_HPP
