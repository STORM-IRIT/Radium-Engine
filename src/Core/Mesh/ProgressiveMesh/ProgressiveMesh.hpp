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

            /// Adjacency
            /// TODO : put it in an other file for use on general DCEL
            void edgeFaceAdjacency(Index edgeIndex, std::vector<Index>& adjOut);
            void vertexFaceAdjacency(Index vertexIndex, std::vector<Index>& adjOut);
            void vertexVertexAdjacency(Index vertexIndex, std::vector<Index>& adjOut);

            /// We construct a priority queue with an error for each edge
            PriorityQueue constructPriorityQueue();
            void updatePriorityQueue(PriorityQueue &pQueue, Index vsId, Index vtId);

            /// Construction of the coarser mesh
            TriangleMesh constructM0(int targetNbFaces);

            /// Compute all faces quadrics
            void computeFacesQuadrics();
            void updateFacesQuadrics(Index vsIndex);

            /// Compute an edge quadric
            Ra::Core::Quadric computeEdgeQuadric(Index edgeIndex);

            /// Compute the error on an edge
            Scalar computeEdgeError(Index edgeIndex, Vector3&p_result);

            ///
            Scalar computeGeometricError(const Vector3& p, Quadric q);

            bool isEcolPossible(Index halfEdgeIndex);

            /// Getters and Setters
            Dcel* getDcel();
            TriangleMesh* getTriangleMesh() const;



            //---------------------
            void test(Index &vs, Index &vt);
            //----------------------

        private:
            Scalar getWedgeAngle(Index faceIndex, Index vsIndex, Index vtIndex);



        private:
            TriangleMesh* m_mesh;
            Dcel* m_dcel;
            Quadric* m_quadrics;
            VSplit* m_vsplits;

        };
    }

}

#endif // PROGRESSIVEMESH_H
