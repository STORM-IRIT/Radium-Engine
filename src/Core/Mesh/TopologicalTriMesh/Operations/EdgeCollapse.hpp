#ifndef EDGECOLLAPSE_H
#define EDGECOLLAPSE_H


#include <Core/RaCore.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Mesh/TopologicalTriMesh/TopologicalMesh.hpp>
#include <Core/Mesh/ProgressiveMesh/ProgressiveMeshData.hpp>

namespace Ra {
namespace Core {
namespace TMOperations {
    RA_CORE_API ProgressiveMeshData edgeCollapse( TopologicalMesh& topologicalMesh, TopologicalMesh::HalfedgeHandle halfEdgeHandle, Vector3 pResult);
    void edgeCollapse( TopologicalMesh& topologicalMesh, ProgressiveMeshData pmData);
}
}
}

#endif // EDGECOLLAPSE_H
