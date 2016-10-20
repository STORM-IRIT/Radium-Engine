#ifndef VERTEXSPLIT_H
#define VERTEXSPLIT_H


#include <Core/RaCore.hpp>
#include <Core/Mesh/DCEL/Dcel.hpp>
#include <Core/Math/LinearAlgebra.hpp>

#include <Core/Mesh/ProgressiveMesh/ProgressiveMeshData.hpp>



namespace Ra {
namespace Core {
namespace DcelOperations {

    //void vertexFaceAdjacency(Dcel& dcel, Index vertexIndex, std::vector<Index>& adjOut);
    void createVt(Dcel& dcel, ProgressiveMeshData pmdata);
    void findFlclwNeig(Dcel& dcel, ProgressiveMeshData pmdata,
                       Index &flclwId, Index &flclwOpId, Index &frcrwId, Index &frcrwOpId,
                       FaceList adjOut);


    RA_CORE_API void vertexSplit(Dcel& dcel, ProgressiveMeshData pmdata);
}
}
}

#endif // EDGECOLLAPSE_H
