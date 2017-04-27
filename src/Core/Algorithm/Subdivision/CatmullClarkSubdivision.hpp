#ifndef RADIUMENGINE_CATMULLCLARK_SUBDIVISION_HPP
#define RADIUMENGINE_CATMULLCLARK_SUBDIVISION_HPP

#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Mesh/HalfEdge.hpp>

namespace Ra {
namespace Core {
namespace Algorithm {

    /// Applies one pass of the Catmull-Clark subdivision scheme to \p mesh.
    /// \note The resulting quads are triangulated.
    /// For more details, see https://en.wikipedia.org/wiki/Catmull%E2%80%93Clark_subdivision_surface
    void CatmullClark( TriangleMesh& mesh, HalfEdgeData& heData );

} // namespace Algorithm
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_CATMULLCLARK_SUBDIVISION_HPP
