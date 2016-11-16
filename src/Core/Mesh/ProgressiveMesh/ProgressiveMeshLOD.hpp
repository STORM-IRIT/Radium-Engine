#ifndef PROGRESSIVEMESHLOD_H
#define PROGRESSIVEMESHLOD_H

#include <Core/Mesh/ProgressiveMesh/ProgressiveMesh.hpp>

namespace Ra
{
    namespace Core
    {
        class ProgressiveMeshLOD
        {
        public:

            /// Constructor
            ProgressiveMeshLOD();
            ProgressiveMeshLOD(ProgressiveMeshBase<>* pm);
            ProgressiveMeshLOD(const ProgressiveMeshLOD &pmlod)
            {
            }
            ~ProgressiveMeshLOD()
            {
            }

            /// This method will build the Progressive Mesh
            TriangleMesh build(int target_nb_faces);

            /// Applies one vertex split :
            /// enables 1 vertex, and 2 faces
            int more();

            /// Applies one edge collapse :
            /// disables 1 vertex, and 2 faces
            int less();

            /// Applies edge collapses or vertex splits
            /// to get the desired level of detail
            TriangleMesh gotoM(int target);
            //TriangleMesh gotoM(Type t, int target_nb_faces);
            //TriangleMesh gotoM(int target_vsplit);



            /// Getters and Setters
            void setNbNoFrVSplit(const int v);
            int getNbNoFrVSplit();
            int getCurrVSplit();
            ProgressiveMeshBase<>* getProgressiveMesh();

        private:
            std::vector<ProgressiveMeshData> m_pmdata;
            ProgressiveMeshBase<>* m_pm;
            int m_nb_no_fr_vsplit;
            int m_curr_vsplit;



        };
    } // namespace Core
} // namespace Ra


#endif // PROGRESSIVEMESHLOD_H
