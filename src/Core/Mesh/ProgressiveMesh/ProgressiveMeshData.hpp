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

            struct DataPerEdgeColor
            {
                Vector3 v0;
                Vector3 v1;
                Scalar error;

                DataPerEdgeColor() : v0(Vector3::Zero()), v1(Vector3::Zero()), error(0.0) {}
                DataPerEdgeColor(Vector3 p0, Vector3 p1, Scalar err) : v0(p0), v1(p1), error(err) {}
                DataPerEdgeColor(const DataPerEdgeColor& data)
                {
                    v0 = data.v0;
                    v1 = data.v1;
                    error = data.error;
                }
            };

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
                   Vector3 q_center, Scalar q_radius,
                   Vector3 vs, Vector3 vt,
                   Vector3 q1_grad, Vector3 q2_grad,
                   std::vector<DataPerEdgeColor> err_per_edge);

            ~ProgressiveMeshData()
            {}

            //---------------------------------------------

            inline Index getHeFlId();
            inline void setHeFlId(Index& i);
            inline Index getHeFrId();
            inline void setHeFrId(Index& i);
            inline Index getFlclwId();
            inline void setFlclwId(Index& i);
            inline Index getFlId();
            inline void setFlId(Index& i);
            inline Index getFrId();
            inline void setFrId(Index& i);
            inline Index getVsId();
            inline void setVsId(Index& i);
            inline Index getVtId();
            inline void setVtId(Index& i);
            inline Index getVlId();
            inline void setVlId(Index& i);
            inline Index getVrId();
            inline void setVrId(Index& i);
            inline short int getii();
            inline void setii(short int &i);
            inline Vector3 getVads();
            inline void setVads(Vector3& v);
            inline Vector3 getVadl();
            inline void setVadl(Vector3& v);

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
            inline Vector3 getVs();
            inline void setVs(const Vector3& c);
            inline Vector3 getVt();
            inline void setVt(const Vector3& c);
            inline Vector3 getGradientQ1();
            inline void setGradientQ1(const Vector3& v);
            inline Vector3 getGradientQ2();
            inline void setGradientQ2(const Vector3& v);
            inline std::vector<DataPerEdgeColor> getErrorPerEdge();
            inline void setErrorPerEdge(const std::vector<DataPerEdgeColor> &v);

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
            Scalar m_error;
            Vector3 m_p_result;
            Vector3 m_q1_center;
            Vector3 m_q2_center;
            Vector3 m_q_center;
            Scalar m_q1_radius;
            Scalar m_q2_radius;
            Scalar m_q_radius;
            Vector3 m_vs;
            Vector3 m_vt;
            Vector3 m_q1_grad;
            Vector3 m_q2_grad;
            std::vector<DataPerEdgeColor> m_error_per_edge;

        };
    } // namespace Core
} // namespace Ra

#include <Core/Mesh/ProgressiveMesh/ProgressiveMeshData.inl>

#endif // PROGRESSIVEMESHDATA_H
