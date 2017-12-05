#ifndef EDGESPLIT_H
#define EDGESPLIT_H

#include <Core/RaCore.hpp>
#include <Core/Mesh/TopologicalTriMesh/TopologicalMesh.hpp>
#include <Core/Index/IndexMap.hpp>
#include <Core/Index/IndexedObject.hpp>

namespace Ra {
namespace Core {
namespace TMOperations {
    void splitEdge( TopologicalMesh& topologicalMesh, TopologicalMesh::EdgeHandle edgeHandle, Scalar fraction );
}
}
}

#endif // EDGESPLIT_H
