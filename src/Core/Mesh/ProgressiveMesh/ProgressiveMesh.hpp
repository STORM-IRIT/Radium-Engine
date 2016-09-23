#ifndef PROGRESSIVEMESH_H
#define PROGRESSIVEMESH_H


#include <Core/Mesh/MeshTypes.hpp>
#include <Core/Mesh/MeshUtils.hpp>
#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Mesh/DCEL/Dcel.hpp>

#include <Core/Mesh/ProgressiveMesh/PriorityQueue.hpp>
#include <Core/Mesh/ProgressiveMesh/VSplit.hpp>

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

            /// We construct a priority queue with an error for each
            /// edge
            PriorityQueue constructPriorityQueue();
            void updatePriorityQueue(PriorityQueue &pQueue, int vsId, int vtId, int edgeId);

            /// Construction of the coarser mesh
            void constructM0(int targetNbFaces);

            /// Compute all faces quadrics
            void computeFacesQuadrics();

            /// Compute an edge quadric
            Ra::Core::Quadric computeEdgeQuadric(Index edgeIndex);

            double computeEdgeError(Index edgeIndex, Vector3&p_result);
            double computeGeometricError(const Vector3& p, Quadric q);

        private:


        private:
            TriangleMesh* m_mesh;
            Dcel* m_dcel;
            Quadric* m_quadrics;
            VSplit* m_vsplits;

        };
    }

}

#endif // PROGRESSIVEMESH_H
