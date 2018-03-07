#ifndef RADIUMENGINE_MESHCONTACTELEMENT_HPP
#define RADIUMENGINE_MESHCONTACTELEMENT_HPP

#include <Engine/Component/Component.hpp>
#include <Engine/Managers/ComponentMessenger/ComponentMessenger.hpp>
#include <Core/File/HandleData.hpp>

#include <Core/Mesh/ProgressiveMesh/ProgressiveMeshLOD.hpp>

#include <Core/TreeStructures/kdtree.hpp>
#include <Core/TreeStructures/trianglekdtree.hpp>

#include <Core/Mesh/ProgressiveMesh/PriorityQueue.hpp>
#include <Core/Mesh/ProgressiveMesh/ProgressiveMesh.hpp>

#include <Core/File/GeometryData.hpp>

#include <Engine/Renderer/Mesh/Mesh.hpp>


typedef Ra::Core::VectorArray<Ra::Core::Triangle> TriangleArray;

namespace Ra
{
    namespace Engine
    {

        class MeshContactElement
        {
        public:

            Super4PCS::TriangleKdTree<>* computeTriangleKdTree(Ra::Core::TriangleMesh& tm);
            void computePrimitives(); // computes initial vertices quadrics used to detect contacts
            void computeFacePrimitives(); // computes initial face quadrics used to detect contacts
            void computeMesh(const std::string& entityName, const std::string& componentName);

            Ra::Core::TriangleMesh getInitTriangleMesh();
            void setTriangleMeshDuplicate();
            Ra::Core::TriangleMesh getTriangleMeshDuplicate();
            void setTriangleMeshDuplicateSimplified();
            Ra::Core::TriangleMesh getTriangleMeshDuplicateSimplified();

            void updateTriangleMesh(Ra::Core::TriangleMesh newMesh);
            bool isConstructM0();
            int getNbFacesMax();

            /// Getters and Setters
            void setPriorityQueue(Ra::Core::PriorityQueue pQueue);
            void setIndex(int idx);
            int getIndex();

            Ra::Core::PriorityQueue* getPriorityQueue();
            Ra::Core::ProgressiveMeshLOD* getProgressiveMeshLOD();
            void setProgressiveMeshLOD(Ra::Core::ProgressiveMeshBase<>* pm);
            Ra::Core::ProgressiveMesh<>::Primitive getPrimitive(int vertexIndex);
            Ra::Core::ProgressiveMesh<>::Primitive getFacePrimitive(int faceIndex);

            Mesh* getMesh();
            void setMesh(const Ra::Core::TriangleMesh& tm);

        private:

            Mesh* m_mesh;

            int m_index;

            Ra::Core::ProgressiveMeshLOD* m_pmlod;
            Ra::Core::PriorityQueue* m_pqueue;
            std::vector<Ra::Core::ProgressiveMesh<>::Primitive> m_primitives; // initial vertex quadrics
            std::vector<Ra::Core::ProgressiveMesh<>::Primitive> m_faceprimitives; // initial face quadrics
            Ra::Core::TriangleMesh m_initTriangleMesh;

            Ra::Core::TriangleMesh m_tm_duplicateVertices;
            Ra::Core::TriangleMesh m_tm_duplicateVerticesSimplified;
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_MESHCONTACTELEMENT_HPP
