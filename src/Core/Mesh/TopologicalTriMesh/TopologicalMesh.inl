#include "TopologicalMesh.hpp"

namespace Ra {
namespace Core {


//
// TopologicalMesh functions
//

inline TopologicalMesh::Normal& TopologicalMesh::normal( TopologicalMesh::VertexHandle vh,
                                                         TopologicalMesh::FaceHandle fh ) {
    // find halfedge that point to vh and member of fh
    return property( halfedge_normals_pph(), halfedge_handle( vh, fh ) );
}
inline TopologicalMesh::HalfedgeHandle
TopologicalMesh::halfedge_handle( TopologicalMesh::VertexHandle vh,
                                  TopologicalMesh::FaceHandle fh ) {
    for ( VertexIHalfedgeIter vih_it = vih_iter( vh ); vih_it.is_valid(); ++vih_it )
    {
        if ( face_handle( *vih_it ) == fh )
        {
            return *vih_it;
        }
    }
    return HalfedgeHandle();
}

} // namespace Core
} // namespace Ra
