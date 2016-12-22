#include <Core/Mesh/ProgressiveMesh/ProgressiveMeshData.hpp>

namespace Ra {
namespace Core {

inline Index ProgressiveMeshData::getHeFlId()
{
    return m_he_fl_id;
}

inline Index ProgressiveMeshData::getHeFrId()
{
    return m_he_fr_id;
}

inline Index ProgressiveMeshData::getFlclwId()
{
    return m_flclw_id;
}

inline Index ProgressiveMeshData::getFlId()
{
    return m_fl_id;
}

inline Index ProgressiveMeshData::getFrId()
{
    return m_fr_id;
}

inline Index ProgressiveMeshData::getVsId()
{
    return m_vs_id;
}

inline Index ProgressiveMeshData::getVtId()
{
    return m_vt_id;
}

inline Index ProgressiveMeshData::getVlId()
{
    return m_vl_id;
}

inline Index ProgressiveMeshData::getVrId()
{
    return m_vr_id;
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

//-----------------------------------------------

inline Scalar ProgressiveMeshData::getError()
{
    return m_error;
}

inline void ProgressiveMeshData::setError(Scalar& s)
{
    m_error = s;
}

inline Vector3 ProgressiveMeshData::getPResult()
{
    return m_p_result;
}

inline void ProgressiveMeshData::setPResult(Vector3& v)
{
    m_p_result = v;
}

inline Vector3 ProgressiveMeshData::getQ1Center()
{
    return m_q1_center;
}

inline void ProgressiveMeshData::setQ1Center(const Vector3 &c)
{
    m_q1_center = c;
}

inline Vector3 ProgressiveMeshData::getQ2Center()
{
    return m_q2_center;
}

inline void ProgressiveMeshData::setQ2Center(const Vector3 &c)
{
    m_q2_center = c;
}

inline Vector3 ProgressiveMeshData::getQCenter()
{
    return m_q_center;
}

inline void ProgressiveMeshData::setQCenter(const Vector3 &c)
{
    m_q_center = c;
}

inline Scalar ProgressiveMeshData::getQ1Radius()
{
    return m_q1_radius;
}

inline void ProgressiveMeshData::setQ1Radius(const Scalar& r)
{
    m_q1_radius = r;
}

inline Scalar ProgressiveMeshData::getQ2Radius()
{
    return m_q2_radius;
}

inline void ProgressiveMeshData::setQ2Radius(const Scalar& r)
{
    m_q2_radius = r;
}

inline Scalar ProgressiveMeshData::getQRadius()
{
    return m_q_radius;
}

inline void ProgressiveMeshData::setQRadius(const Scalar& r)
{
    m_q_radius = r;
}

} // namespace Core
} // namespace Ra
