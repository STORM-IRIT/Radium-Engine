#include <Core/Mesh/ProgressiveMesh/ProgressiveMeshData.hpp>

namespace Ra {
namespace Core {

inline Index ProgressiveMeshData::getHeFlId()
{
    return m_he_fl_id;
}

inline void ProgressiveMeshData::setHeFlId(Index &i)
{
    m_he_fl_id = i;
}

inline Index ProgressiveMeshData::getHeFrId()
{
    return m_he_fr_id;
}

inline void ProgressiveMeshData::setHeFrId(Index &i)
{
    m_he_fr_id = i;
}

inline Index ProgressiveMeshData::getFlclwId()
{
    return m_flclw_id;
}

inline void ProgressiveMeshData::setFlclwId(Index &i)
{
    m_flclw_id = i;
}

inline Index ProgressiveMeshData::getFlId()
{
    return m_fl_id;
}

inline void ProgressiveMeshData::setFlId(Index &i)
{
    m_fl_id = i;
}

inline Index ProgressiveMeshData::getFrId()
{
    return m_fr_id;
}

inline void ProgressiveMeshData::setFrId(Index &i)
{
    m_fr_id = i;
}

inline Index ProgressiveMeshData::getVsId()
{
    return m_vs_id;
}

inline void ProgressiveMeshData::setVsId(Index &i)
{
    m_vs_id = i;
}

inline Index ProgressiveMeshData::getVtId()
{
    return m_vt_id;
}

inline void ProgressiveMeshData::setVtId(Index &i)
{
    m_vt_id = i;
}

inline Index ProgressiveMeshData::getVlId()
{
    return m_vl_id;
}

inline void ProgressiveMeshData::setVlId(Index &i)
{
    m_vl_id = i;
}

inline Index ProgressiveMeshData::getVrId()
{
    return m_vr_id;
}

inline void ProgressiveMeshData::setVrId(Index &i)
{
    m_vr_id = i;
}

inline short int ProgressiveMeshData::getii()
{
    return m_ii;
}

inline void ProgressiveMeshData::setii(short &i)
{
    m_ii = i;
}

inline Vector3 ProgressiveMeshData::getVads()
{
    return m_vad_s;
}

inline void ProgressiveMeshData::setVads(Vector3 &v)
{
    m_vad_s = v;
}

inline Vector3 ProgressiveMeshData::getVadl()
{
    return m_vad_l;
}

inline void ProgressiveMeshData::setVadl(Vector3 &v)
{
    m_vad_l = v;
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

inline Vector3 ProgressiveMeshData::getVs()
{
    return m_vs;
}

inline void ProgressiveMeshData::setVs(const Vector3& c)
{
    m_vs = c;
}

inline Vector3 ProgressiveMeshData::getVt()
{
    return m_vt;
}

inline void ProgressiveMeshData::setVt(const Vector3& c)
{
    m_vt = c;
}

inline Vector3 ProgressiveMeshData::getGradientQ1()
{
    return m_q1_grad;
}

inline void ProgressiveMeshData::setGradientQ1(const Vector3& v)
{
    m_q1_grad = v;
}

inline Vector3 ProgressiveMeshData::getGradientQ2()
{
    return m_q2_grad;
}

inline void ProgressiveMeshData::setGradientQ2(const Vector3& v)
{
    m_q2_grad = v;
}


} // namespace Core
} // namespace Ra
