#ifndef EDGECOLLAPSE_H
#define EDGECOLLAPSE_H


#include <Core/RaCore.hpp>

#include <Core/Math/LinearAlgebra.hpp>

#include <Core/Mesh/DCEL/Dcel.hpp>
#include <Core/Mesh/ProgressiveMesh/ProgressiveMeshData.hpp>

namespace Ra {
namespace Core {
namespace DcelOperations {
    RA_CORE_API void edgeCollapse( Dcel& dcel, Index edgeIndex, Vector3 pResult, bool updatePMData, ProgressiveMeshData& data);
    void edgeCollapse( Dcel& dcel, ProgressiveMeshData pmData);
}
}
}

#endif // EDGECOLLAPSE_H
