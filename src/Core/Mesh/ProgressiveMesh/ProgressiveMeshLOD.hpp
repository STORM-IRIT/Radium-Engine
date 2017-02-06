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
            void build(int target_nb_faces, int primitive_update, float scale, int weight_per_vertex, TriangleMesh &m, std::ofstream &file);

            /// Applies one vertex split :
            /// enables 1 vertex, and 2 faces
            int more();

            /// Applies one edge collapse :
            /// disables 1 vertex, and 2 faces
            int less();

            /// Applies edge collapses or vertex splits
            /// to get the desired level of detail
            void gotoM(int target, TriangleMesh &newMesh);
            //TriangleMesh gotoM(Type t, int target_nb_faces);
            //TriangleMesh gotoM(int target_vsplit);

            /// Getters and Setters
            void setNbNoFrVSplit(const int v);
            int getNbNoFrVSplit();
            int getCurrVSplit();
            int getNbPMData();
            ProgressiveMeshData* getCurrPMData();
            ProgressiveMeshBase<>* getProgressiveMesh();
            std::vector<ProgressiveMeshData> getProgressiveMeshDataVector();
            ProgressiveMeshData getPMData(int i);

        private:
            std::vector<ProgressiveMeshData> m_pmdata;
            ProgressiveMeshBase<>* m_pm;
            int m_nb_no_fr_vsplit;
            int m_curr_vsplit;



        };
    } // namespace Core
} // namespace Ra


#endif // PROGRESSIVEMESHLOD_H
