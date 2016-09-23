#include "VSplit.hpp"

namespace Ra
{
    namespace Core
    {

        VSplit::VSplit()
        {
            m_vad_l = Vector3::Zero();
            m_vad_s = Vector3::Zero();
            m_flclw_id = -1;
            m_fl_id = -1;
            m_fr_id = -1;
            m_vs_id = -1;
            m_vt_id = -1;
            m_vl_id = -1;
            m_vr_id = -1;
            m_vs_id_fl = -1;
            m_vs_id_fr = -1;
            m_ii = 0;
        }

        VSplit::VSplit(const Vector3& vad_l, const Vector3& vad_s,
                       int flclw_id, int fl_id, int fr_id,
                       int vs_id, int vt_id, int vl_id, int vr_id,
                       int vs_id_fl, int vs_id_fr,
                       short int ii)
        {
            m_vad_l = vad_l;
            m_vad_s = vad_s;
            m_flclw_id = flclw_id;
            m_fl_id = fl_id;
            m_fr_id = fr_id;
            m_vs_id = vs_id;
            m_vt_id = vt_id;
            m_vl_id = vl_id;
            m_vr_id = vr_id;
            m_vs_id_fl = vs_id_fl;
            m_vs_id_fr = vs_id_fr;
            m_ii = ii;

        }

        Vector3 VSplit::computePResult(const Vector3& vt, const Vector3& vs)
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

