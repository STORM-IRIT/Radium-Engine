#include <Core/Mesh/ProgressiveMesh/ProgressiveMeshLOD.hpp>
#include <Core/Mesh/Wrapper/Convert.hpp>
#include <Core/Log/Log.hpp>

namespace Ra
{
    namespace Core
    {

        ProgressiveMeshLOD::ProgressiveMeshLOD()
        {
            m_curr_vsplit = 0;
            m_nb_no_fr_vsplit = 0;
        }

        ProgressiveMeshLOD::ProgressiveMeshLOD(ProgressiveMeshBase<>* pm)
        {
            m_pm = pm;
            m_curr_vsplit = 0;
            m_nb_no_fr_vsplit = 0;
        }

        //------------------------------

        TriangleMesh ProgressiveMeshLOD::build(int target_nb_faces)
        {
            m_pmdata = m_pm->constructM0(target_nb_faces, m_nb_no_fr_vsplit);
            m_curr_vsplit = m_pmdata.size();

            TriangleMesh newMesh;
            convertPM(*(m_pm->getDcel()), newMesh);
            return newMesh;
        }

        //------------------------------

        int ProgressiveMeshLOD::more()
        {
            // applies the next vertex split transformation
            if (m_curr_vsplit > 0)
            {
                m_pm->vsplit(m_pmdata[--m_curr_vsplit]);
                return 1;
            }
            else
                return 0;
        }

        int ProgressiveMeshLOD::less()
        {
            // applies one edge collapse transformation
            if (m_curr_vsplit < m_pmdata.size())
            {
                m_pm->ecol(m_pmdata[m_curr_vsplit++]);
                return 1;
            }
            else
                return 0;
        }

        //------------------------------

        TriangleMesh ProgressiveMeshLOD::gotoM(int target)
        {
            if (m_pm->getNbFaces() < target)
            {
                while (m_pm->getNbFaces() < target)
                {
                    if (!more())
                        break;
                }
            }
            else if (m_pm->getNbFaces() > target)
            {
                while (m_pm->getNbFaces() > target)
                {
                    if (!less())
                        break;
                }
            }
            TriangleMesh newMesh;
            convertPM(*(m_pm->getDcel()), newMesh);
            return newMesh;
        }

        /*
        TriangleMesh ProgressiveMeshLOD::gotoM(Type t, int target)
        {
            int targetVSplit;
            int nbVSplit = m_pmdata.size();
            float ratioNoFrVSplit = (float)m_nb_no_fr_vsplit / (float)nbVSplit;
            switch(t)
            {
            case WANT_NVERTICES:
                targetVSplit = m_pm->getNbFaces() - target;
                break;
            case WANT_NFACES:
                int nbFaces = m_pm->getNbFaces();
                targetVSplit = (nbFaces - target) * ratioNoFrVSplit + 0.5 * (nbFaces - target) * (1.0 - ratioNoFrVSplit);
                break;
            }

            return gotoM(targetVSplit);
        }

        TriangleMesh ProgressiveMeshLOD::gotoM(int targetVSplit)
        {
            while(m_curr_vsplit != targetVSplit)
            {
                if (m_curr_vsplit < targetVSplit)
                {
                    if (!less())
                        break;
                }
                else if (m_curr_vsplit > targetVSplit)
                {
                    if (!more())
                        break;
                }
            }
            TriangleMesh newMesh;
            convertPM(*(m_pm->getDcel()), newMesh);
            return newMesh;
        }
        */

        //------------------------------

        void ProgressiveMeshLOD::setNbNoFrVSplit(const int v)
        {
            m_nb_no_fr_vsplit = v;
        }

        int ProgressiveMeshLOD::getNbNoFrVSplit()
        {
            return m_nb_no_fr_vsplit;
        }

        int ProgressiveMeshLOD::getCurrVSplit()
        {
            return m_curr_vsplit;
        }

        ProgressiveMeshBase<>* ProgressiveMeshLOD::getProgressiveMesh()
        {
            return m_pm;
        }
    } // Core
} // Ra
