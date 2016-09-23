#ifndef VSPLIT_H
#define VSPLIT_H

#include <Core/Math/LinearAlgebra.hpp>

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
        class VSplit
        {
        public:

            VSplit();

            VSplit(const Vector3& vad_l, const Vector3& vad_s,
                   int flclw_id, int fl_id, int fr_id,
                   int vs_id, int vt_id, int vl_id, int vr_id,
                   int vs_id_fl, int vs_id_fr,
                   short int ii);

            ~VSplit()
            {}

            //---------------------------------------------

            inline int getFlclw();
            inline int getFl();
            inline int getFr();
            inline int getVsId();
            inline int getVtId();
            inline int getVlId();
            inline int getVrId();
            inline int getVsIdFl();
            inline int getVsIdFr();
            inline short int getii();
            inline Vector3 getVads();
            inline Vector3 getVadl();

            //----------------------------------------------

            Vector3 computePResult(const Vector3& vt, const Vector3& vs);


        private:

            Vector3 m_vad_l, m_vad_s;

            // identify the location of a vertex split
            int m_flclw_id; // a face in vsplit's neibourghood
            int m_fl_id, m_fr_id;
            int m_vs_id, m_vt_id, m_vl_id, m_vr_id;
            int m_vs_id_fl, m_vs_id_fr;

            // gives the attribute of the new vertex
            short int m_ii; // prediction of p position
        };
    } // namespace Core
} // namespace Ra

#endif // VSPLIT_H
