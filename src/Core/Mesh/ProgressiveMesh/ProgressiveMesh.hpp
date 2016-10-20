#ifndef PROGRESSIVEMESH_H
#define PROGRESSIVEMESH_H


#include <Core/Mesh/MeshTypes.hpp>
#include <Core/Mesh/MeshUtils.hpp>
#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Mesh/DCEL/Dcel.hpp>

#include <Core/Mesh/ProgressiveMesh/PriorityQueue.hpp>
#include <Core/Mesh/ProgressiveMesh/ProgressiveMeshData.hpp>

#include <Core/Math/Quadric.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Math/Quadric.hpp>

#include <Core/Index/Index.hpp>

#include <Core/Containers/VectorArray.hpp>

namespace Ra
{
    namespace Core
    {

        class ProgressiveMesh
        {
        public:
            ProgressiveMesh(TriangleMesh* mesh);
            ProgressiveMesh(const ProgressiveMesh& mesh) {}

            ~ProgressiveMesh() {}

            /// We construct a priority queue with an error for each edge
            PriorityQueue constructPriorityQueue();
            void updatePriorityQueue(PriorityQueue &pQueue, Index vsId, Index vtId);

            /// Construction of the coarser mesh
            std::vector<ProgressiveMeshData> constructM0(int targetNbFaces, int &nbNoFrVSplit);

            /// Vertex Split
            void vsplit(ProgressiveMeshData pmData);
            void ecol(ProgressiveMeshData pmData);

            /// Compute all faces quadrics
            void computeFacesQuadrics();
            void updateFacesQuadrics(Index vsIndex);

            /// Compute an edge quadric
            Ra::Core::Quadric computeEdgeQuadric(Index edgeIndex);

            /// Compute the error on an edge
            Scalar computeEdgeError(Index edgeIndex, Vector3&p_result);

            ///
            Scalar computeGeometricError(const Vector3& p, Quadric q);

            bool isEcolPossible(Index halfEdgeIndex, Vector3 pResult);

            /// Getters and Setters
            inline Dcel* getDcel();
            inline int getNbFaces();
            inline int getNbVertices();

        private:
            Scalar getWedgeAngle(Index faceIndex, Index vsIndex, Index vtIndex);



        private:
            Dcel* m_dcel;
            Quadric* m_quadrics;
            int m_nb_faces;
            int m_nb_vertices;
        };
    }

}

#include <Core/Mesh/ProgressiveMesh/ProgressiveMesh.inl>

#endif // PROGRESSIVEMESH_H
