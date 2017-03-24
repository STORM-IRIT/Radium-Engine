#ifndef PROGRESSIVEMESHDATA_H
#define PROGRESSIVEMESHDATA_H

#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Index/Index.hpp>

#include <Core/Math/Quadric.hpp>

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
                   short int ii /*, ProgressiveMesh<>::Primitive qVt*/);

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
            inline void setQVt(Ra::Core::Quadric<3> qVt);
            inline Ra::Core::Quadric<3> getQVt();
//            inline void setQVt(ProgressiveMesh<>::Primitive qVt);
//            inline ProgressiveMesh<>::Primitive getQVt();

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

            Ra::Core::Quadric<3> m_qVt;
//            ProgressiveMesh<>::Primitive m_qVt;
        };
    } // namespace Core
} // namespace Ra

#include <Core/Mesh/ProgressiveMesh/ProgressiveMeshData.inl>

#endif // PROGRESSIVEMESHDATA_H
