#ifndef EDGESPLIT_H
#define EDGESPLIT_H

#include <Core/Index/IndexMap.hpp>
#include <Core/Index/IndexedObject.hpp>
#include <Core/Mesh/TopologicalTriMesh/TopologicalMesh.hpp>
#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {
namespace TMOperations {
void splitEdge( TopologicalMesh& topologicalMesh, TopologicalMesh::EdgeHandle edgeHandle,
                Scalar fraction );
}
} // namespace Core
} // namespace Ra

#endif // EDGESPLIT_H
