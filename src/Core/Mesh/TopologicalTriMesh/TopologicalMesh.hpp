#ifndef TOPOLOGICALMESH_H
#define TOPOLOGICALMESH_H

#include <Core/Mesh/TopologicalTriMesh/OpenMesh.h>

#include <Core/Index/Index.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/RaCore.hpp>

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/Traits.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Utils/Property.hh>

#include <Eigen/Core>
#include <Eigen/Geometry>

namespace Ra {
namespace Core {

class TriangleMesh;

/// Define the Traits to be used by OpenMesh for TopologicalMesh.
struct TopologicalMeshTraits : OpenMesh::DefaultTraits {
    using Point = Ra::Core::Vector3;
    using Normal = Ra::Core::Vector3;

    VertexAttributes( OpenMesh::Attributes::Status | OpenMesh::Attributes::Normal );
    FaceAttributes( OpenMesh::Attributes::Status | OpenMesh::Attributes::Normal );
    EdgeAttributes( OpenMesh::Attributes::Status );
    HalfedgeAttributes( OpenMesh::Attributes::Status | OpenMesh::Attributes::Normal );
};

/// This class represents a mesh with topological information on the
/// vertex graph, using a half-edge representation.
///
/// This integration is inspired by: https://gist.github.com/Unril/03fa353d0461ed6bd41d
class RA_CORE_API TopologicalMesh : public OpenMesh::PolyMesh_ArrayKernelT<TopologicalMeshTraits> {
  private:
    using base = OpenMesh::PolyMesh_ArrayKernelT<TopologicalMeshTraits>;
    using base::PolyMesh_ArrayKernelT;

    std::vector<OpenMesh::HPropHandleT<float>> m_floatPph;
    std::vector<OpenMesh::HPropHandleT<Vector2>> m_vec2Pph;
    std::vector<OpenMesh::HPropHandleT<Vector3>> m_vec3Pph;
    std::vector<OpenMesh::HPropHandleT<Vector4>> m_vec4Pph;

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    /// Construct a topological mesh from a triangle mesh.
    /// This operation merges vertices with same position, but keeps vertex
    /// attributes on halfedges, so that TriangleMesh vertices with the same 3D
    /// position are represented only once in the topological mesh.
    /// \note this is a costly operation.
    explicit TopologicalMesh( const Ra::Core::TriangleMesh& triMesh );

    /// Construct an empty topological mesh.
    explicit TopologicalMesh(){};

    /// Obtain a triangleMesh from a topological mesh.
    /// This function is non-const because of the computation of face normals.
    /// \note this is a costly operation.
    TriangleMesh toTriangleMesh();

    // import other version of halfedge_handle method.
    using base::halfedge_handle;

    /// Return the half-edge associated with a given vertex and face.
    inline HalfedgeHandle halfedge_handle( VertexHandle vh, FaceHandle fh );

    /// Return the normal of the vertex \p vh, when considering its membership to
    /// the face \p fh.
    inline Normal& normal( VertexHandle vh, FaceHandle fh );
};
} // namespace Core
} // namespace Ra

#include <Core/Mesh/TopologicalTriMesh/TopologicalMesh.inl>

#endif // TOPOLOGICALMESH_H
