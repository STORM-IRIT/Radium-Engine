#ifndef TOPOLOGICALMESH_H
#define TOPOLOGICALMESH_H

#include <Core/Index/Index.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/RaCore.hpp>

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/Traits.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Utils/Property.hh>

namespace Ra {
namespace Core {

///\class TopoVector3 : small extension to Vector3 for OpenMesh compatibility
class TopoVector3 : public Ra::Core::Vector3 {
  public:
    using Ra::Core::Vector3::Vector3;
    inline Scalar length() const { return norm(); }
    inline Scalar sqrnorm() const { return squaredNorm(); }
    inline TopoVector3 vectorize( Scalar v ) {
        ( *this )[0] = v;
        ( *this )[1] = v;
        ( *this )[2] = v;
        return *this;
    }
    inline TopoVector3 normalize() {
        Ra::Core::Vector3::normalize();
        return *this;
    }
};

inline Scalar dot( const TopoVector3& a, const TopoVector3& b ) {
    return a.dot( b );
}
inline TopoVector3 cross( const TopoVector3& a, const TopoVector3& b ) {
    return a.cross( b );
}
} // namespace Core
} // namespace Ra

template <>
struct OpenMesh::vector_traits<Ra::Core::TopoVector3> {
    /// Type of the vector class
    typedef typename Ra::Core::Vector3 vector_type;

    /// Type of the scalar value
    typedef Scalar value_type;

    /// size/dimension of the vector
    static const size_t size_ = 3;

    /// size/dimension of the vector
    static inline size_t size() { return size_; }
};

namespace Ra {
namespace Core {

// Define an OpenMesh TriMesh structure according to
// http://openmesh.org/Documentation/OpenMesh-2.1-Documentation/mesh_type.html
// Attributes define data store on structure.

struct TopologicalMeshTraits : public OpenMesh::DefaultTraits {
    typedef TopoVector3 Point;
    typedef TopoVector3 Normal;

    VertexAttributes( OpenMesh::Attributes::Status | OpenMesh::Attributes::Normal );
    FaceAttributes( OpenMesh::Attributes::Status | OpenMesh::Attributes::Normal );
    EdgeAttributes( OpenMesh::Attributes::Status );
    HalfedgeAttributes( OpenMesh::Attributes::Status | OpenMesh::Attributes::Normal );

    VertexTraits {
      private:
        Index m_index;

      public:
        /// If index valid, normal is TriMesh vertex normal
        inline Index getIndex() { return m_index; }
        inline void setIndex( Index index ) { m_index = index; }
    };

    HalfedgeTraits {
      private:
        Index m_index;

      public:
        /// if Index valid, normal and other data of halfedge is TriMesh vertex data
        inline Index getIndex() { return m_index; }
        inline void setIndex( Index index ) { m_index = index; }
    };

  public:
};

class TopologicalMesh : public OpenMesh::PolyMesh_ArrayKernelT<TopologicalMeshTraits> {
    using base = OpenMesh::PolyMesh_ArrayKernelT<TopologicalMeshTraits>;
    using base::PolyMesh_ArrayKernelT;

  public:
    inline Normal& normal( VertexHandle vh, FaceHandle fh ) {
        // find halfedge that point to vh and member of fh
        return property( halfedge_normals_pph(), halfedge_handle( vh, fh ) );
    }

    using base::halfedge_handle;
    inline HalfedgeHandle halfedge_handle( VertexHandle vh, FaceHandle fh ) {

        for ( VertexIHalfedgeIter vih_it = vih_iter( vh ); vih_it.is_valid(); ++vih_it )
        {
            if ( face_handle( *vih_it ) == fh )
            {
                return *vih_it;
            }
        }
        return HalfedgeHandle();
    }
};
} // namespace Core
} // namespace Ra

#endif // TOPOLOGICALMESH_H
