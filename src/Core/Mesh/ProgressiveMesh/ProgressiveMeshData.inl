#include <Core/Mesh/ProgressiveMesh/ProgressiveMeshData.hpp>

namespace Ra {
namespace Core {

inline Index ProgressiveMeshData::getHeFlId() const
{
    return m_he_fl_id;
}

inline Index ProgressiveMeshData::getHeFrId() const
{
    return m_he_fr_id;
}

inline Index ProgressiveMeshData::getFlclwId() const
{
    return m_flclw_id;
}

inline Index ProgressiveMeshData::getFlId() const
{
    return m_fl_id;
}

inline Index ProgressiveMeshData::getFrId() const
{
    return m_fr_id;
}

inline Index ProgressiveMeshData::getVsId() const
{
    return m_vs_id;
}

inline Index ProgressiveMeshData::getVtId() const
{
    return m_vt_id;
}

inline Index ProgressiveMeshData::getVlId() const
{
    return m_vl_id;
}

inline Index ProgressiveMeshData::getVrId() const
{
    return m_vr_id;
}

inline short int ProgressiveMeshData::getii() const
{
    return m_ii;
}

inline Vector3 ProgressiveMeshData::getVads() const
{
    return m_vad_s;
}

inline Vector3 ProgressiveMeshData::getVadl() const
{
    return m_vad_l;
}

} // namespace Core
} // namespace Ra
