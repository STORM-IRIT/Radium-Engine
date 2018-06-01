#include "TopologicalMesh.hpp"

namespace Ra {
namespace Core {

//
// TopoVector3 functions
//

inline Scalar TopoVector3::length() const {
    return norm();
}
inline Scalar TopoVector3::sqrnorm() const {
    return squaredNorm();
}
inline TopoVector3 TopoVector3::vectorize( Scalar v ) {
    ( *this )[0] = v;
    ( *this )[1] = v;
    ( *this )[2] = v;
    return *this;
}
inline TopoVector3 TopoVector3::normalize() {
    Ra::Core::Vector3::normalize();
    return *this;
}
inline Scalar dot( const TopoVector3& a, const TopoVector3& b ) {
    return a.dot( b );
}
inline TopoVector3 cross( const TopoVector3& a, const TopoVector3& b ) {
    return a.cross( b );
}

inline TopoVector3& normalize( TopoVector3& v ) {
    return v = v.normalized();
}

inline Scalar sqrnorm( const TopoVector3& v ) {
    return v.sqrnorm();
}

inline Scalar norm( const TopoVector3& v ) {
    return v.norm();
}

template <typename OtherScalar>
inline void vectorize( TopoVector3& v, const OtherScalar s ) {
    v.fill( s );
}

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
