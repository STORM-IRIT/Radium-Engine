#include "ProgressiveMesh.hpp"

namespace Ra
{
    namespace Core
    {

        inline Dcel* ProgressiveMesh::getDcel()
        {
            return m_dcel;
        }

        inline int ProgressiveMesh::getNbFaces()
        {
            return m_nb_faces;
        }

        inline int ProgressiveMesh::getNbVertices()
        {
            return m_nb_vertices;
        }


    }

}
