#ifndef PROGRESSIVEMESH_H
#define PROGRESSIVEMESH_H


#include <Core/Mesh/MeshTypes.hpp>
#include <Core/Mesh/MeshUtils.hpp>
#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Mesh/DCEL/Dcel.hpp>

#include <Core/Mesh/ProgressiveMesh/PriorityQueue.hpp>
#include <Core/Mesh/ProgressiveMesh/ProgressiveMeshData.hpp>
#include <Core/Mesh/ProgressiveMesh/ErrorMetric.hpp>

#include <Core/Math/Quadric.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Math/Quadric.hpp>

#include <Core/Index/Index.hpp>

#include <Core/Containers/VectorArray.hpp>

namespace Ra
{
    namespace Core
    {

//      template<class ErrorMetric = QuadricErrorMetric>
        template<class ErrorMetric = SimpleAPSSErrorMetric>
        class ProgressiveMeshBase
        {
        public:

            using Primitive = typename ErrorMetric::Primitive;

            virtual std::vector<ProgressiveMeshData> constructM0(int targetNbFaces, int &nbNoFrVSplit) = 0;
            virtual void computeFacesQuadrics() = 0;
            virtual Primitive computeEdgeQuadric(Index edgeIndex) = 0;
            virtual void vsplit(ProgressiveMeshData pmData) = 0;
            virtual void ecol(ProgressiveMeshData pmData) = 0;

            virtual Dcel* getDcel() = 0;
            virtual int getNbFaces() = 0;
            virtual ErrorMetric getEM() = 0;
        };

//      template<class ErrorMetric = QuadricErrorMetric>
        template<class ErrorMetric = SimpleAPSSErrorMetric>
        class ProgressiveMesh : public ProgressiveMeshBase<ErrorMetric>
        {

        public:

            using Primitive = typename ErrorMetric::Primitive;

            ProgressiveMesh(TriangleMesh* mesh);
            ProgressiveMesh(const ProgressiveMesh& mesh) {}

            ~ProgressiveMesh() {}

            /// We construct a priority queue with an error for each edge
            PriorityQueue constructPriorityQueue();
            void updatePriorityQueue(PriorityQueue &pQueue, Index vsId, Index vtId);

            /// Construction of the coarser mesh
            std::vector<ProgressiveMeshData> constructM0(int targetNbFaces, int &nbNoFrVSplit) override;

            /// Vertex Split
            void vsplit(ProgressiveMeshData pmData) override;
            void ecol(ProgressiveMeshData pmData) override;

            /// Compute all faces quadrics
            inline void computeFacesQuadrics();
            void updateFacesQuadrics(Index vsIndex);

            /// Compute an edge quadric
            Primitive computeEdgeQuadric(Index edgeIndex);

            /// Compute the error on an edge
            Scalar computeEdgeError(Index edgeIndex, Vector3&p_result);

            ///
            Scalar computeGeometricError(const Vector3& p, Primitive q);

            bool isEcolPossible(Index halfEdgeIndex, Vector3 pResult);

            Scalar computeBoundingBoxSize();

            /// Getters and Setters
            inline Dcel* getDcel();
            inline int getNbFaces();
            inline ErrorMetric getEM();

        private:
            Scalar getWedgeAngle(Index faceIndex, Index vsIndex, Index vtIndex);



        private:
            Dcel* m_dcel;
            std::vector<Primitive> m_primitives;
            ErrorMetric m_em;
            Scalar m_bbox_size;
            int m_nb_faces;
            int m_nb_vertices;
        };
    }

}

#include <Core/Mesh/ProgressiveMesh/ProgressiveMesh.inl>

#endif // PROGRESSIVEMESH_H
