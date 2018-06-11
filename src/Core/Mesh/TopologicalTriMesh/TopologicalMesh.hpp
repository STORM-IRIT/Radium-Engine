#ifndef TOPOLOGICALMESH_H
#define TOPOLOGICALMESH_H

#include <Core/Index/Index.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/RaCore.hpp>

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/Traits.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Utils/Property.hh>

namespace Ra
{
namespace Core
{

class TriangleMesh;

///\class TopoVector3 : small extension to Vector3 for OpenMesh compatibility
class RA_CORE_API TopoVector3 : public Ra::Core::Vector3
{
  public:
    using Ra::Core::Vector3::Vector3;
    inline Scalar length() const;
    inline Scalar sqrnorm() const;
    inline TopoVector3 vectorize( Scalar v );
    inline TopoVector3 normalize();
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

inline RA_CORE_API Scalar dot( const TopoVector3& a, const TopoVector3& b );
inline RA_CORE_API TopoVector3 cross( const TopoVector3& a, const TopoVector3& b );
inline RA_CORE_API TopoVector3& normalize( TopoVector3& v );
inline RA_CORE_API Scalar sqrnorm( const TopoVector3& v );
inline RA_CORE_API Scalar norm( const TopoVector3& v );
template <typename OtherScalar>
inline void vectorize( TopoVector3& v, const OtherScalar s );
} // namespace Core
} // namespace Ra

template <>
struct RA_CORE_API OpenMesh::vector_traits<Ra::Core::TopoVector3>
{
    /// Type of the vector class
    using vector_type = Ra::Core::Vector3;

    /// Type of the scalar value
    using value_type = Scalar;

    /// size/dimension of the vector
    static const size_t size_ = 3;

    /// size/dimension of the vector
    static inline size_t size() { return size_; }

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

namespace Ra
{
namespace Core
{

// Define an OpenMesh TriMesh structure according to
// http://openmesh.org/Documentation/OpenMesh-2.1-Documentation/mesh_type.html
// Attributes define data store on structure.

struct RA_CORE_API TopologicalMeshTraits : public OpenMesh::DefaultTraits
{
    using Point = TopoVector3;
    using Normal = TopoVector3;

    VertexAttributes( OpenMesh::Attributes::Status | OpenMesh::Attributes::Normal );
    FaceAttributes( OpenMesh::Attributes::Status | OpenMesh::Attributes::Normal );
    EdgeAttributes( OpenMesh::Attributes::Status );
    HalfedgeAttributes( OpenMesh::Attributes::Status | OpenMesh::Attributes::Normal );

    VertexTraits
    {
      private:
        Index m_index;

      public:
        /// If index valid, normal is TriMesh vertex normal
        inline Index getIndex() { return m_index; }
        inline void setIndex( Index index ) { m_index = index; }
    };

    HalfedgeTraits
    {
      private:
        Index m_index;

      public:
        /// if Index valid, normal and other data of halfedge is TriMesh vertex data
        inline Index getIndex() { return m_index; }
        inline void setIndex( Index index ) { m_index = index; }
    };

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

/// This class represent a mesh with topological information on the
/// vertex graph, using a half-edge representation.
class RA_CORE_API TopologicalMesh : public OpenMesh::PolyMesh_ArrayKernelT<TopologicalMeshTraits>
{
  private:
    using base = OpenMesh::PolyMesh_ArrayKernelT<TopologicalMeshTraits>;
    using base::PolyMesh_ArrayKernelT;

    std::vector<OpenMesh::HPropHandleT<float>> m_floatPph;
    std::vector<OpenMesh::HPropHandleT<Vector2>> m_vec2Pph;
    std::vector<OpenMesh::HPropHandleT<Vector3>> m_vec3Pph;
    std::vector<OpenMesh::HPropHandleT<Vector4>> m_vec4Pph;

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    /// Construct a topological mesh from a triangle mesh
    /// this is a costly operation.
    /// This operation merge vertex with same position, but keep vertex
    /// attributes on halfedge, so that triMesh vertex with same 3D position are
    /// represented only once in the topological mesh.
    explicit TopologicalMesh( const Ra::Core::TriangleMesh& triMesh );

    /// Construct an empty topological mesh
    explicit TopologicalMesh(){};

    /// Obtain a triangleMesh from a topological mesh.
    /// This is a costly operation.
    /// This function is non-const because of the computation of face normals.
    TriangleMesh toTriangleMesh();

    // import other version of halfedge_handle method
    using base::halfedge_handle;
    /// Return the half-edge associated with a given vertex and face.
    inline HalfedgeHandle halfedge_handle( VertexHandle vh, FaceHandle fh );

    /// return the normal of the vertex vh, when considering its membership to
    /// the face fh
    inline Normal& normal( VertexHandle vh, FaceHandle fh );
};
} // namespace Core
} // namespace Ra

#include <Core/Mesh/TopologicalTriMesh/TopologicalMesh.inl>
#endif // TOPOLOGICALMESH_H
