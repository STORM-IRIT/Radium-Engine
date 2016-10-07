#include "ProgressiveMesh.hpp"

namespace Ra
{
    namespace Core
    {

        inline Dcel* ProgressiveMesh::getDcel()
        {
            return m_dcel;
        }

        inline TriangleMesh* ProgressiveMesh::getTriangleMesh() const
        {
            return m_mesh;
        }

        //-----------------------------------------------------------


    }

}
