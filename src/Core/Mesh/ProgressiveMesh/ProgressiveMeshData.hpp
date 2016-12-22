#ifndef PROGRESSIVEMESHDATA_H
#define PROGRESSIVEMESHDATA_H

#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Index/Index.hpp>

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
                   Index he_fl_id, Index he_fr_id,
                   Index flclw_id, Index fl_id, Index fr_id,
                   Index vs_id, Index vt_id, Index vl_id, Index vr_id,
                   short int ii);

            ProgressiveMeshData(const Vector3& vad_l, const Vector3& vad_s,
                   Index he_fl_id, Index he_fr_id,
                   Index flclw_id, Index fl_id, Index fr_id,
                   Index vs_id, Index vt_id, Index vl_id, Index vr_id,
                   short int ii,
                   Scalar error, Vector3 p_result,
                   Vector3 q1_center, Scalar q1_radius,
                   Vector3 q2_center, Scalar q2_radius,
                   Vector3 q_center, Scalar q_radius);

            ~ProgressiveMeshData()
            {}

            //---------------------------------------------

            inline Index getHeFlId();
            inline Index getHeFrId();
            inline Index getFlclwId();
            inline Index getFlId();
            inline Index getFrId();
            inline Index getVsId();
            inline Index getVtId();
            inline Index getVlId();
            inline Index getVrId();
            inline short int getii();
            inline Vector3 getVads();
            inline Vector3 getVadl();

            //----------------------------------------------

            inline Scalar getError();
            inline void setError(Scalar& s);
            inline Vector3 getPResult();
            inline void setPResult(Vector3& v);
            inline Vector3 getQ1Center();
            inline void setQ1Center(const Vector3& c);
            inline Vector3 getQ2Center();
            inline void setQ2Center(const Vector3& c);
            inline Vector3 getQCenter();
            inline void setQCenter(const Vector3& c);
            inline Scalar getQ1Radius();
            inline void setQ1Radius(const Scalar& r);
            inline Scalar getQ2Radius();
            inline void setQ2Radius(const Scalar& r);
            inline Scalar getQRadius();
            inline void setQRadius(const Scalar& r);

            //----------------------------------------------

            Vector3 computePResult(const Vector3& vt, const Vector3& vs);


        private:

            Vector3 m_vad_l, m_vad_s;

            // identify the location of a vertex split
            Index m_he_fl_id, m_he_fr_id;
            Index m_flclw_id; // a face in vsplit's neibourghood (containing vs-vl)
            Index m_fl_id, m_fr_id;
            Index m_vs_id, m_vt_id, m_vl_id, m_vr_id;

            // gives the attribute of the new vertex
            short int m_ii; // prediction of p position

            ///////////////////////////////////
            /// only for debug interactions ///
            ///////////////////////////////////
            // TODO add
            Scalar m_error;
            Vector3 m_p_result;
            Vector3 m_q1_center;
            Vector3 m_q2_center;
            Vector3 m_q_center;
            Scalar m_q1_radius;
            Scalar m_q2_radius;
            Scalar m_q_radius;
        };
    } // namespace Core
} // namespace Ra

#include <Core/Mesh/ProgressiveMesh/ProgressiveMeshData.inl>

#endif // PROGRESSIVEMESHDATA_H
