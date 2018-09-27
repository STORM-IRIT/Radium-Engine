#include "TopologicalMesh.hpp"

namespace Ra {
namespace Core {

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
