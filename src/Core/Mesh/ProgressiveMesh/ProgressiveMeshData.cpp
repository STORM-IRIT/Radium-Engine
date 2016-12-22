#include "ProgressiveMeshData.hpp"

namespace Ra
{
    namespace Core
    {

        ProgressiveMeshData::ProgressiveMeshData()
        {
            m_vad_l = Vector3::Zero();
            m_vad_s = Vector3::Zero();
            m_he_fl_id = -1;
            m_he_fr_id = -1;
            m_flclw_id = -1;
            m_fl_id = -1;
            m_fr_id = -1;
            m_vs_id = -1;
            m_vt_id = -1;
            m_vl_id = -1;
            m_vr_id = -1;
            m_ii = 0;

            m_error = 0.0;
            m_q1_center = Vector3::Zero();
            m_q2_center = Vector3::Zero();
            m_q_center = Vector3::Zero();
            m_q1_radius = 0.0;
            m_q2_radius = 0.0;
            m_q_radius = 0.0;
        }

        ProgressiveMeshData::ProgressiveMeshData(const Vector3& vad_l, const Vector3& vad_s,
                       Index he_fl_id, Index he_fr_id,
                       Index flclw_id, Index fl_id, Index fr_id,
                       Index vs_id, Index vt_id, Index vl_id, Index vr_id,
                       short int ii)
        {
            m_vad_l = vad_l;
            m_vad_s = vad_s;
            m_he_fl_id = he_fl_id;
            m_he_fr_id = he_fr_id;
            m_flclw_id = flclw_id;
            m_fl_id = fl_id;
            m_fr_id = fr_id;
            m_vs_id = vs_id;
            m_vt_id = vt_id;
            m_vl_id = vl_id;
            m_vr_id = vr_id;
            m_ii = ii;
        }

        ProgressiveMeshData::ProgressiveMeshData(const Vector3& vad_l, const Vector3& vad_s,
                       Index he_fl_id, Index he_fr_id,
                       Index flclw_id, Index fl_id, Index fr_id,
                       Index vs_id, Index vt_id, Index vl_id, Index vr_id,
                       short int ii, Scalar error, Vector3 p_result,
                       Vector3 q1_center, Scalar q1_radius,
                       Vector3 q2_center, Scalar q2_radius, Vector3 q_center, Scalar q_radius)
        {
            m_vad_l = vad_l;
            m_vad_s = vad_s;
            m_he_fl_id = he_fl_id;
            m_he_fr_id = he_fr_id;
            m_flclw_id = flclw_id;
            m_fl_id = fl_id;
            m_fr_id = fr_id;
            m_vs_id = vs_id;
            m_vt_id = vt_id;
            m_vl_id = vl_id;
            m_vr_id = vr_id;
            m_ii = ii;

            m_error = error;
            m_p_result = p_result;
            m_q1_center = q1_center;
            m_q2_center = q2_center;
            m_q_center = q_center;
            m_q1_radius = q1_radius;
            m_q2_radius = q2_radius;
            m_q_radius = q_radius;
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

