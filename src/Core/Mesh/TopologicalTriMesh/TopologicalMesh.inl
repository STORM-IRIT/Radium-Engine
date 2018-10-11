#include "TopologicalMesh.hpp"

namespace Ra {
namespace Core {

inline const TopologicalMesh::Normal&
TopologicalMesh::normal( TopologicalMesh::VertexHandle vh, TopologicalMesh::FaceHandle fh ) const {
    // find halfedge that point to vh and member of fh
    return normal( halfedge_handle( vh, fh ) );
}

inline void TopologicalMesh::set_normal( TopologicalMesh::VertexHandle vh,
                                         TopologicalMesh::FaceHandle fh,
                                         const TopologicalMesh::Normal& n ) {

    set_normal( halfedge_handle( vh, fh ), n );
}

inline void TopologicalMesh::propagate_normal_to_halfedges( TopologicalMesh::VertexHandle vh ) {
    for ( VertexIHalfedgeIter vih_it = vih_iter( vh ); vih_it.is_valid(); ++vih_it )
    {
        set_normal( *vih_it, normal( vh ) );
    }
}

inline TopologicalMesh::HalfedgeHandle
TopologicalMesh::halfedge_handle( TopologicalMesh::VertexHandle vh,
                                  TopologicalMesh::FaceHandle fh ) const {
    for ( ConstVertexIHalfedgeIter vih_it = cvih_iter( vh ); vih_it.is_valid(); ++vih_it )
    {
        if ( face_handle( *vih_it ) == fh )
        {
            return *vih_it;
        }
    }
    CORE_ASSERT( false, "vh is not a vertex of face fh" );
    return HalfedgeHandle();
}

inline const std::vector<OpenMesh::HPropHandleT<float>>&
TopologicalMesh::getFloatPropsHandles() const {
    return m_floatPph;
}

inline const std::vector<OpenMesh::HPropHandleT<Vector2>>&
TopologicalMesh::getVector2PropsHandles() const {
    return m_vec2Pph;
}

inline const std::vector<OpenMesh::HPropHandleT<Vector3>>&
TopologicalMesh::getVector3PropsHandles() const {
    return m_vec3Pph;
}

inline const std::vector<OpenMesh::HPropHandleT<Vector4>>&
TopologicalMesh::getVector4PropsHandles() const {
    return m_vec4Pph;
}

} // namespace Core
} // namespace Ra
