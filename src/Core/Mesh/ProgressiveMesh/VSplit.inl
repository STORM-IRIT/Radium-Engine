#include <Core/Mesh/ProgressiveMesh/VSplit.hpp>

namespace Ra {
namespace Core {


inline int getFlclw()
{
    return m_flclw_id;
}

inline int getFl()
{
    return m_fl_id;
}

inline int getFr()
{
    return m_fr_id;
}

inline int getVsId()
{
    return m_vs_id;
}

inline int getVtId()
{
    return m_vt_id;
}

inline int getVlId()
{
    return m_vl_id;
}

inline int getVrId()
{
    return m_vr_id;
}

inline int getVsIdFl()
{
    return m_vs_id_fl;
}

inline int getVsIdFr()
{
    return m_vs_id_fr;
}

inline short int getii()
{
    return m_ii;
}

inline Vector3 getVads()
{
    return m_vad_s;
}

inline Vector3 getVadl()
{
    return m_vad_l;
}

} // namespace Core
} // namespace Ra
