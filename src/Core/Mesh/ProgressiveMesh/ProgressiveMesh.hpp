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
            virtual ProgressiveMeshData constructM0(int &nbNoFrVSplit, std::vector<Super4PCS::KdTree<float>*> kdtrees, int idx, PriorityQueue &pQueue) = 0;
            virtual bool isConstructM0(std::vector<Super4PCS::KdTree<float>*> kdtrees, int idx, PriorityQueue &pQueue) = 0;

            //virtual PriorityQueue constructPriorityQueue(std::vector<Super4PCS::KdTree<float>*> kdtrees, int objIndex) = 0;

            virtual void computeFacesQuadrics() = 0;
            virtual std::vector<Primitive> getFacesQuadrics() = 0;
            virtual void updateFacesQuadrics(Index vsIndex) = 0;
            virtual Primitive computeEdgeQuadric(Index edgeIndex) = 0;
            //virtual Primitive computeVertexQuadric(Index vertexIndex) = 0;

            virtual int vertexContact(Index vertexIndex, std::vector<Super4PCS::KdTree<float>*> kdtrees, int idxOtherObject) = 0;

            virtual bool isEcolConsistent(Index halfEdgeIndex, Vector3 pResult) = 0;
            virtual bool isEcolPossible(Index halfEdgeIndex, Vector3 pResult/*, std::vector<Super4PCS::KdTree<float>*> kdtrees, int idx*/) = 0;

            virtual Scalar computeEdgeErrorContact(Index halfEdgeIndex, Vector3 &pResult, Primitive qc) = 0;

            virtual void vsplit(ProgressiveMeshData pmData) = 0;
            virtual void ecol(ProgressiveMeshData pmData) = 0;

            virtual Dcel* getDcel() = 0;
            virtual int getNbFaces() = 0;
            virtual ErrorMetric getEM() = 0;

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
            void updatePriorityQueue(std::vector<Super4PCS::KdTree<float>*> kdtrees, PriorityQueue &pQueue, Index vsId, Index vtId, int objIndex);

            /// Construction of the coarser mesh
            //std::vector<ProgressiveMeshData> constructM0(int targetNbFaces, int &nbNoFrVSplit, std::vector<Super4PCS::KdTree<float>*> kdtrees, int idx) override;
            ProgressiveMeshData constructM0(int &nbNoFrVSplit, std::vector<Super4PCS::KdTree<float>*> kdtrees, int idx, PriorityQueue &pQueue) override;
            bool isConstructM0(std::vector<Super4PCS::KdTree<float>*> kdtrees, int idx, PriorityQueue &pQueue) override;




            /// Vertex Split
            void vsplit(ProgressiveMeshData pmData) override;
            void ecol(ProgressiveMeshData pmData) override;

            /// Compute all faces quadrics
            inline void computeFacesQuadrics();
            std::vector<Primitive> getFacesQuadrics();
            void updateFacesQuadrics(Index vsIndex);

            /// Compute an edge quadric
            Primitive computeEdgeQuadric(Index edgeIndex);

            ///Compute a vertex quadric
            //Primitive computeVertexQuadric(Index vertexIndex);

            /// Compute the error on an edge
            Scalar computeEdgeError(Index edgeIndex, Vector3&p_result);
            Scalar computeEdgeErrorContact(Index halfEdgeIndex, Vector3 &pResult, Primitive qc);

            ///
            Scalar computeGeometricError(const Vector3& p, Primitive q);

            int vertexContact(Index vertexIndex, std::vector<Super4PCS::KdTree<float>*> kdtrees, int idxOtherObject);
            bool hasContact(Index halfEdgeIndex, std::vector<Super4PCS::KdTree<float>*> kdtrees, int idx);

            bool isEcolConsistent(Index halfEdgeIndex, Vector3 pResult);
            bool isEcolPossible(Index halfEdgeIndex, Vector3 pResult/*, std::vector<Super4PCS::KdTree<float>*> kdtrees, int idx*/);

            Scalar computeBoundingBoxSize();

            /// Getters and Setters
            inline Dcel* getDcel();
            inline int getNbFaces();
            inline ErrorMetric getEM();

            inline void collapseFace();
            inline void collapseVertex();

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
