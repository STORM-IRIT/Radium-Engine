#ifndef EDGESPLIT_H
#define EDGESPLIT_H

#include <Core/Index/IndexMap.hpp>
#include <Core/Index/IndexedObject.hpp>
#include <Core/Mesh/TopologicalTriMesh/TopologicalMesh.hpp>
#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {
namespace TMOperations {

/// Perform the edge split operation on edge \p edgeHandle of \p mesh.
/// The newly inserted vertice is set as the linear interpolation between
/// the edge endpoints according to \p fraction.
void RA_CORE_API splitEdge( TopologicalMesh& mesh, TopologicalMesh::EdgeHandle edgeHandle,
                            Scalar fraction );

} // namespace TMOperations
} // namespace Core
} // namespace Ra

#endif // EDGESPLIT_H
