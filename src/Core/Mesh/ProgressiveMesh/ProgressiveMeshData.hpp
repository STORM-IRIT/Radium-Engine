#ifndef PROGRESSIVEMESHDATA_H
#define PROGRESSIVEMESHDATA_H

#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Mesh/TopologicalTriMesh/TopologicalMesh.hpp>
#include <Core/Index/Index.hpp>

#include <Core/Math/Quadric.hpp>

/**
 * Class VSplit
 * The class VSplit contains all the information needed for
 * a vertex split or an edge collapse
 *
 * Reference from:
 * "Efficient Implementation of Progressive Meshes"
 * [ Hugues Hoppe ]
 * "http://hhoppe.com/efficientpm.pdf"
 */

namespace Ra
{
    namespace Core
    {
        class ProgressiveMeshData
        {
        public:

            ProgressiveMeshData();

            ProgressiveMeshData(const Vector3& vad_l, const Vector3& vad_s,
                           TopologicalMesh::HalfedgeHandle he_fl, TopologicalMesh::HalfedgeHandle he_fr,
                           TopologicalMesh::FaceHandle flclw, TopologicalMesh::FaceHandle fl, TopologicalMesh::FaceHandle fr,
                           TopologicalMesh::VertexHandle vs, TopologicalMesh::VertexHandle vt, TopologicalMesh::VertexHandle vl, TopologicalMesh::VertexHandle vr,
                           short int ii/*, ProgressiveMesh<>::Primitive qVt*/);

            ~ProgressiveMeshData()
            {}

            //---------------------------------------------

            inline TopologicalMesh::HalfedgeHandle getHeFl();
            inline TopologicalMesh::HalfedgeHandle getHeFr();

            inline void setHeFl(TopologicalMesh::HalfedgeHandle newHefl);
            inline void setHeFr(TopologicalMesh::HalfedgeHandle newHefr);

            inline TopologicalMesh::FaceHandle getFlclw();
            inline TopologicalMesh::FaceHandle getFl();
            inline TopologicalMesh::FaceHandle getFr();
            inline TopologicalMesh::VertexHandle getVs();
            inline TopologicalMesh::VertexHandle getVt();
            inline TopologicalMesh::VertexHandle getVl();
            inline TopologicalMesh::VertexHandle getVr();
            inline short int getii();
            inline Vector3 getVads();
            inline Vector3 getVadl();
            inline void setQVt(Ra::Core::Quadric<3> qVt);
            inline Ra::Core::Quadric<3> getQVt();
//            inline void setQVt(ProgressiveMesh<>::Primitive qVt);
//            inline ProgressiveMesh<>::Primitive getQVt();

            //----------------------------------------------

            Vector3 computePResult(const Vector3& vt, const Vector3& vs);


        private:

            Vector3 m_vad_l, m_vad_s;

            // identify the location of a vertex split
            TopologicalMesh::HalfedgeHandle m_he_fl, m_he_fr;
            TopologicalMesh::FaceHandle m_flclw; // a face in vsplit's neibourghood (containing vs-vl)
            TopologicalMesh::FaceHandle m_fl, m_fr;
            TopologicalMesh::VertexHandle m_vs, m_vt, m_vl, m_vr;

            // gives the attribute of the new vertex
            short int m_ii; // prediction of p position

            Ra::Core::Quadric<3> m_qVt;
//            ProgressiveMesh<>::Primitive m_qVt;
        };
    } // namespace Core
} // namespace Ra

#include <Core/Mesh/ProgressiveMesh/ProgressiveMeshData.inl>

#endif // PROGRESSIVEMESHDATA_H
