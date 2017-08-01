#include "ProgressiveMeshData.hpp"

namespace Ra
{
    namespace Core
    {

        ProgressiveMeshData::ProgressiveMeshData()
        {
            m_vad_l = Vector3::Zero();
            m_vad_s = Vector3::Zero();
            m_he_fl = TopologicalMesh::HalfedgeHandle();
            m_he_fr = TopologicalMesh::HalfedgeHandle();
            m_flclw = TopologicalMesh::FaceHandle();
            m_fl = TopologicalMesh::FaceHandle();
            m_fr = TopologicalMesh::FaceHandle();
            m_vs = TopologicalMesh::VertexHandle();
            m_vt = TopologicalMesh::VertexHandle();
            m_vl = TopologicalMesh::VertexHandle();
            m_vr = TopologicalMesh::VertexHandle();
            m_ii = 0;
            //m_qVt = Ra::Core::Quadric();
        }

        ProgressiveMeshData::ProgressiveMeshData(const Vector3& vad_l, const Vector3& vad_s,
                       TopologicalMesh::HalfedgeHandle he_fl, TopologicalMesh::HalfedgeHandle he_fr,
                       TopologicalMesh::FaceHandle flclw, TopologicalMesh::FaceHandle fl, TopologicalMesh::FaceHandle fr,
                       TopologicalMesh::VertexHandle vs, TopologicalMesh::VertexHandle vt, TopologicalMesh::VertexHandle vl, TopologicalMesh::VertexHandle vr,
                       short int ii/*, ProgressiveMesh<>::Primitive qVt*/)
        {
            m_vad_l = vad_l;
            m_vad_s = vad_s;
            m_he_fl = he_fl;
            m_he_fr = he_fr;
            m_flclw = flclw;
            m_fl = fl;
            m_fr = fr;
            m_vs = vs;
            m_vt = vt;
            m_vl = vl;
            m_vr = vr;
            m_ii = ii;
            //m_qVt(qVt);
        }

        Vector3 ProgressiveMeshData::computePResult(const Vector3& vt, const Vector3& vs)
        {
            Vector3 p_result;
            if (m_ii == 0)
                p_result = vt - m_vad_s;
            else if (m_ii == 1)
                p_result = vt - m_vad_l;
            else
                p_result = ((vt + vs) / 2.f) - m_vad_s;
            return p_result;
        }
    } // namespace Core
} // namespace Ra

