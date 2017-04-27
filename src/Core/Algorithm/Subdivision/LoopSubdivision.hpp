#ifndef RADIUMENGINE_LOOP_SUBDIVISION_HPP
#define RADIUMENGINE_LOOP_SUBDIVISION_HPP

#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Mesh/HalfEdge.hpp>

namespace Ra {
namespace Core {
namespace Algorithm {

    /// Applies one pass of the Loop subdivision scheme to \p mesh.
    /// For more details, see https://graphics.stanford.edu/~mdfisher/subdivision.html
    void LoopSubdivision( TriangleMesh& mesh, HalfEdgeData& heData );

} // namespace Algorithm
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_LOOP_SUBDIVISION_HPP
