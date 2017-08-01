#include <Core/Mesh/ProgressiveMesh/ProgressiveMeshData.hpp>

namespace Ra {
namespace Core {

inline TopologicalMesh::HalfedgeHandle ProgressiveMeshData::getHeFl()
{
    return m_he_fl;
}

inline TopologicalMesh::HalfedgeHandle ProgressiveMeshData::getHeFr()
{
    return m_he_fr;
}

inline void ProgressiveMeshData::setHeFl(TopologicalMesh::HalfedgeHandle newHefl)
{
    m_he_fl = newHefl;
}

inline void ProgressiveMeshData::setHeFr(TopologicalMesh::HalfedgeHandle newHefr)
{
    m_he_fr = newHefr;
}

inline TopologicalMesh::FaceHandle ProgressiveMeshData::getFlclw()
{
    return m_flclw;
}

inline TopologicalMesh::FaceHandle ProgressiveMeshData::getFl()
{
    return m_fl;
}

inline TopologicalMesh::FaceHandle ProgressiveMeshData::getFr()
{
    return m_fr;
}

inline TopologicalMesh::VertexHandle ProgressiveMeshData::getVs()
{
    return m_vs;
}

inline TopologicalMesh::VertexHandle ProgressiveMeshData::getVt()
{
    return m_vt;
}

inline TopologicalMesh::VertexHandle ProgressiveMeshData::getVl()
{
    return m_vl;
}

inline TopologicalMesh::VertexHandle ProgressiveMeshData::getVr()
{
    return m_vr;
}

inline short int ProgressiveMeshData::getii()
{
    return m_ii;
}

inline Vector3 ProgressiveMeshData::getVads()
{
    return m_vad_s;
}

inline Vector3 ProgressiveMeshData::getVadl()
{
    return m_vad_l;
}

inline void ProgressiveMeshData::setQVt(Ra::Core::Quadric<3> qVt)
{
    m_qVt = Quadric<3>(qVt);
}

inline Ra::Core::Quadric<3> ProgressiveMeshData::getQVt()
{
    return m_qVt;
}

} // namespace Core
} // namespace Ra
