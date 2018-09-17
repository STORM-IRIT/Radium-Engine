#ifndef EDGESPLIT_H
#define EDGESPLIT_H

#include <Core/Index/IndexMap.hpp>
#include <Core/Index/IndexedObject.hpp>
#include <Core/Mesh/TopologicalTriMesh/TopologicalMesh.hpp>
#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {

class TMOperations {
  public:
    /**
     * \brief Apply a 2-4 edge split.
     * \param mesh The mesh on which to apply the edge split.
     * \param eh The handle to the edge to split.
     * \param f The interpolation factor to place the new point on the edge.
     *          Must be in [0,1].
     * \return True if the edge has been split, false otherwise.
     * \note Only applies on edges between 3 triangles, and if \p f is in [0,1].
     * \note Mesh attributes are linearly interpolated on the newly created halfedge.
     */
    static bool splitEdge( TopologicalMesh& mesh, TopologicalMesh::EdgeHandle eh, Scalar f );
};

} // namespace Core
} // namespace Ra

#endif // EDGESPLIT_H
