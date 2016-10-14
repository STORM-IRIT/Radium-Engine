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

} // namespace Core
} // namespace Ra
