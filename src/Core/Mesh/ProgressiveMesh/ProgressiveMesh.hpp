#ifndef PROGRESSIVEMESH_H
#define PROGRESSIVEMESH_H


#include <Core/Mesh/MeshTypes.hpp>
#include <Core/Mesh/MeshUtils.hpp>
#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Mesh/TopologicalTriMesh/TopologicalMesh.hpp>
#include <Core/Mesh/DCEL/Dcel.hpp>

#include <Core/Mesh/ProgressiveMesh/PriorityQueue.hpp>
#include <Core/Mesh/ProgressiveMesh/ProgressiveMeshData.hpp>
#include <Core/Mesh/ProgressiveMesh/ErrorMetric.hpp>

#include <Core/Math/Quadric.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Math/Quadric.hpp>

#include <Core/Index/Index.hpp>

#include <Core/Containers/VectorArray.hpp>

#include <Core/TreeStructures/kdtree.hpp>

namespace Ra
{
    namespace Core
    {

    //class ProgressiveMeshLOD;

      template<class ErrorMetric = QuadricErrorMetric>
//        template<class ErrorMetric = SimpleAPSSErrorMetric>
        class ProgressiveMeshBase
        {
        public:

            using Primitive = typename ErrorMetric::Primitive;

//            virtual std::vector<ProgressiveMeshData> constructM0(int targetNbFaces, int &nbNoFrVSplit, std::vector<Super4PCS::KdTree<float>*> kdtrees, int idx) = 0;
            //virtual ProgressiveMeshData constructM0(int &nbNoFrVSplit, std::vector<Super4PCS::KdTree<float>*> kdtrees, int idx, PriorityQueue &pQueue) = 0;
            virtual bool isConstructM0(std::vector<Super4PCS::KdTree<Scalar>*> kdtrees, int idx, PriorityQueue &pQueue) = 0;

            //virtual PriorityQueue constructPriorityQueue(std::vector<Super4PCS::KdTree<float>*> kdtrees, int objIndex) = 0;

            virtual void computeFacesQuadrics() = 0;
            virtual void updateFacesQuadrics(TopologicalMesh::VertexHandle vsHandle) = 0;
            virtual Primitive computeEdgeQuadric(TopologicalMesh::HalfedgeHandle edgeHandle) = 0;
            virtual Primitive computeVertexQuadric(TopologicalMesh::VertexHandle vertexHandle) = 0;

            virtual int vertexContact(TopologicalMesh::VertexHandle vertexHandle, std::vector<Super4PCS::KdTree<Scalar>*> kdtrees, int idxOtherObject, Scalar threshold) = 0;

            virtual bool isEcolConsistent(TopologicalMesh::HalfedgeHandle halfEdgeHandle, Vector3 pResult) = 0;
            virtual bool isEcolPossible(TopologicalMesh::HalfedgeHandle halfEdgeHandle, Vector3 pResult/*, std::vector<Super4PCS::KdTree<float>*> kdtrees, int idx*/) = 0;

            virtual bool isPlanarEdge(TopologicalMesh::HalfedgeHandle halfEdgeHandle) = 0;
            virtual bool isPlanarEdge2(TopologicalMesh::HalfedgeHandle halfEdgeHandle, TopologicalMesh::VertexHandle &vsHandle, TopologicalMesh::VertexHandle &vtHandle) = 0;

            virtual Scalar computeEdgeErrorContact(TopologicalMesh::HalfedgeHandle halfEdgeHandle, Vector3 &pResult, Primitive qc) = 0;

            virtual void vsplit(ProgressiveMeshData pmData) = 0;
            virtual void ecol(ProgressiveMeshData pmData) = 0;

            virtual TopologicalMesh* getTopologicalMesh() = 0;
            virtual int getNbFaces() = 0;
            virtual ErrorMetric getEM() = 0;
            virtual std::vector<Primitive> getFacesQuadrics() = 0;

            virtual void collapseFace() = 0;
            virtual void collapseVertex() = 0;
        };

      template<class ErrorMetric = QuadricErrorMetric>
//        template<class ErrorMetric = SimpleAPSSErrorMetric>
        class ProgressiveMesh : public ProgressiveMeshBase<ErrorMetric>
        {

        public:

            using Primitive = typename ErrorMetric::Primitive;

            ProgressiveMesh(TriangleMesh* mesh);
            ProgressiveMesh(const ProgressiveMesh& mesh) {}

            ~ProgressiveMesh() {}

            /// We construct a priority queue with an error for each edge
            //PriorityQueue constructPriorityQueue(std::vector<Super4PCS::KdTree<float>*> kdtrees, int objIndex);
            //void updatePriorityQueue(std::vector<Super4PCS::KdTree<float>*> kdtrees, PriorityQueue &pQueue, Index vsId, Index vtId, int objIndex);

            /// Construction of the coarser mesh
            //std::vector<ProgressiveMeshData> constructM0(int targetNbFaces, int &nbNoFrVSplit, std::vector<Super4PCS::KdTree<float>*> kdtrees, int idx) override;
            //ProgressiveMeshData constructM0(int &nbNoFrVSplit, std::vector<Super4PCS::KdTree<float>*> kdtrees, int idx, PriorityQueue &pQueue) override;
            bool isConstructM0(std::vector<Super4PCS::KdTree<Scalar>*> kdtrees, int idx, PriorityQueue &pQueue) override;




            /// Vertex Split
            void vsplit(ProgressiveMeshData pmData) override;
            void ecol(ProgressiveMeshData pmData) override;

            /// Compute all faces quadrics
            inline void computeFacesQuadrics();
            void updateFacesQuadrics(TopologicalMesh::VertexHandle vsHandle);

            /// Compute an edge quadric
            Primitive computeEdgeQuadric(TopologicalMesh::HalfedgeHandle halfEdgeHandle);

            ///Compute a vertex quadric
            Primitive computeVertexQuadric(TopologicalMesh::VertexHandle vertexHandle);

            /// Compute the error on an edge
            Scalar computeEdgeError(TopologicalMesh::HalfedgeHandle halfEdgeHandle, Vector3&p_result);
            Scalar computeEdgeErrorContact(TopologicalMesh::HalfedgeHandle halfEdgeHandle, Vector3 &pResult, Primitive qc);

            ///
            Scalar computeGeometricError(const Vector3& p, Primitive q);

            int vertexContact(TopologicalMesh::VertexHandle vertexHandle, std::vector<Super4PCS::KdTree<Scalar>*> kdtrees, int idxOtherObject, Scalar threshold);
            //bool hasContact(Index halfEdgeIndex, std::vector<Super4PCS::KdTree<Scalar>*> kdtrees, int idx);

            bool isEcolConsistent(TopologicalMesh::HalfedgeHandle halfEdgeHandle, Vector3 pResult);
            bool isEcolPossible(TopologicalMesh::HalfedgeHandle halfEdgeHandle, Vector3 pResult/*, std::vector<Super4PCS::KdTree<float>*> kdtrees, int idx*/);

            bool isPlanarEdge(TopologicalMesh::HalfedgeHandle halfEdgeHandle);
            bool isPlanarEdge2(TopologicalMesh::HalfedgeHandle halfEdgeHandle, TopologicalMesh::VertexHandle &vsHandle, TopologicalMesh::VertexHandle &vtHandle);

            Scalar computeBoundingBoxSize();

            /// Getters and Setters
            inline TopologicalMesh* getTopologicalMesh();
            inline int getNbFaces();
            inline ErrorMetric getEM();
            inline std::vector<Primitive> getFacesQuadrics();

            inline void collapseFace();
            inline void collapseVertex();

        private:
            Scalar getWedgeAngle(TopologicalMesh::FaceHandle faceHandle, TopologicalMesh::VertexHandle vsHandle, TopologicalMesh::VertexHandle vtHandle);



        private:
            TopologicalMesh* m_topologicalMesh;
            //Dcel* m_dcel;
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
