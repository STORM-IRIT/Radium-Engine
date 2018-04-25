#ifndef EDGESPLIT_H
#define EDGESPLIT_H

#include <Core/Container/IndexMap.hpp>
#include <Core/Container/IndexedObject.hpp>
#include <Core/Geometry/TopologicalMesh.hpp>
#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {
namespace Geometry {
void splitEdge( TopologicalMesh& topologicalMesh, TopologicalMesh::EdgeHandle edgeHandle,
                Scalar fraction );
} // namespace Geometry
} // namespace Core
} // namespace Ra

#endif // EDGESPLIT_H
