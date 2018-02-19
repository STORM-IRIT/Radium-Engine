#include <Core/Mesh/TopologicalTriMesh/TopologicalMesh.hpp>

namespace Ra {
namespace Core {

Scalar dot( const TopoVector3& a, const TopoVector3& b ) {
    return a.dot( b );
}
TopoVector3 cross( const TopoVector3& a, const TopoVector3& b ) {
    return a.cross( b );
}
} // namespace Core
} // namespace Ra
