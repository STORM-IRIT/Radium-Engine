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

#include <iostream>
#include <fstream>

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

            virtual std::vector<ProgressiveMeshData> constructM0(int targetNbFaces, int &nbNoFrVSplit, int primitiveUpdate, float scale, int weight_per_vertex) = 0;
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
            std::vector<ProgressiveMeshData> constructM0(int targetNbFaces, int &nbNoFrVSplit, int primitiveUpdate, float scale, int weight_per_vertex) override;

            /// Vertex Split and Edge Collapse
            void vsplit(ProgressiveMeshData pmData) override;
            void ecol(ProgressiveMeshData pmData) override;

            /// Compute all faces quadrics
            inline void computeFacesQuadrics();
            void updateFacesQuadrics(Index vsIndex, HalfEdge_ptr he);

            /// ComputeVertexQuadric
            Primitive computeVertexQuadric(Index vertexIndex);

            /// Compute an edge quadric
            Primitive computeEdgeQuadric(Index edgeIndex);

            /// Compute the error on an edge
            Scalar computeEdgeError(Index edgeIndex, Vector3&p_result, Primitive &q, std::ofstream &file);
            Scalar computeEdgeError(Index edgeIndex, Vector3&p_result, Primitive &q);

            /// Check if an edge collapse is doable
            bool isEcolPossible(Index halfEdgeIndex, Vector3 pResult);

            /// Compute the size of the bounding box of a mesh
            Scalar computeBoundingBoxSize();

            /// Getters and Setters
            inline Dcel* getDcel();
            inline int getNbFaces();
            inline ErrorMetric getEM();

        private:
            Scalar getWedgeAngle(Index faceIndex, Index vIndex);



        private:
            Dcel* m_dcel;
            std::vector<Primitive> m_primitives;
            std::vector<Primitive> m_primitives_he; //TO REMOVE
            std::vector<Primitive> m_primitives_v;
            ErrorMetric m_em;

            Scalar m_bbox_size;
            Scalar m_mean_edge_size;
            Scalar m_scale;
            int m_weight_per_vertex;
            int m_primitive_update;

            int m_nb_faces;
            int m_nb_vertices;
        };
    }

}

#include <Core/Mesh/ProgressiveMesh/ProgressiveMesh.inl>

#endif // PROGRESSIVEMESH_H
