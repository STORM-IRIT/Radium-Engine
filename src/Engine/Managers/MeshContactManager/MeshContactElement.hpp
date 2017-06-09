#ifndef RADIUMENGINE_MESHCONTACTELEMENT_HPP
#define RADIUMENGINE_MESHCONTACTELEMENT_HPP

#include <Engine/Component/Component.hpp>
#include <Engine/Managers/ComponentMessenger/ComponentMessenger.hpp>
#include <Engine/Assets/HandleData.hpp>

#include <Core/Mesh/ProgressiveMesh/ProgressiveMeshLOD.hpp>

#include <Core/TreeStructures/kdtree.hpp>

#include <Core/Mesh/ProgressiveMesh/PriorityQueue.hpp>
#include <Core/Mesh/ProgressiveMesh/ProgressiveMesh.hpp>

#include <Engine/Assets/GeometryData.hpp>


typedef Ra::Core::VectorArray<Ra::Core::Triangle> TriangleArray;

namespace Ra
{
    namespace Engine
    {

        class MeshContactElement
        {
        public:

            void addMesh(
                    ComponentMessenger::CallbackTypes<Ra::Core::Vector3Array>::ReadWrite verticesWriter,
                    ComponentMessenger::CallbackTypes<Ra::Core::Vector3Array>::ReadWrite normalsWriter,
                    ComponentMessenger::CallbackTypes<TriangleArray>::ReadWrite trianglesWriter
                    );

            Super4PCS::KdTree<float>* computeKdTree();
            void computePrimitives(); //Computes initial vertices quadrics to be used to detect contacts
            void computeProgressiveMesh();
            void updateTriangleMesh(Ra::Core::TriangleMesh newMesh);
            void lodValueChanged(int value);
            bool isConstructM0();
            int getNbFacesMax();

            /// Getters and Setters
            void setPriorityQueue(Ra::Core::PriorityQueue pQueue);
            void setIndex(int idx);
            int getIndex();

            Ra::Core::PriorityQueue* getPriorityQueue();
            Ra::Core::ProgressiveMeshLOD* getProgressiveMeshLOD();
            Ra::Core::ProgressiveMesh<>::Primitive getPrimitive(int vertexIndex);

        private:
            ComponentMessenger::CallbackTypes<Ra::Core::Vector3Array>::ReadWrite m_verticesWriter;
            ComponentMessenger::CallbackTypes<Ra::Core::Vector3Array>::ReadWrite m_normalsWriter;
            ComponentMessenger::CallbackTypes<TriangleArray>::ReadWrite m_trianglesWriter;

            int m_index;

            Ra::Core::ProgressiveMeshLOD* m_pmlod;
            Ra::Core::PriorityQueue* m_pqueue;
            std::vector<Ra::Core::ProgressiveMesh<>::Primitive> m_primitives; // Initial vertex quadrics
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_MESHCONTACTELEMENT_HPP
