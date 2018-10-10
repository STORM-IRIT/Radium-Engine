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

    /// Construct a topological mesh from a triangle mesh
    /// this is a costly operation.
    /// This operation merge vertex with same position, but keep vertex
    /// attributes on halfedge, so that triMesh vertices with the same 3D position
    /// are represented only once in the topological mesh.
    explicit TopologicalMesh( const Ra::Core::TriangleMesh& triMesh );

    /// Construct an empty topological mesh
    explicit TopologicalMesh() {}

    /// Return a triangleMesh from the topological mesh.
    /// This is a costly operation.
    /// \warning It uses the attributs defined on halfedges.
    TriangleMesh toTriangleMesh() const;

    // import other version of halfedge_handle method
    using base::halfedge_handle;

    /// Return the half-edge associated with a given vertex and face.
    /// assert if vh is not a member of fh
    inline HalfedgeHandle halfedge_handle( VertexHandle vh, FaceHandle fh ) const;

    /// Get normal of the vertex vh, when member of fh
    /// assert if vh is not a member of fh
    inline const Normal& normal( VertexHandle vh, FaceHandle fh ) const;

    /// Set normal of the vertex vh, when member of fh
    /// assert if vh is not a member of fh
    void set_normal( VertexHandle vh, FaceHandle fh, const Normal& n );

    /// import Base definition of normal and set normal
    ///@{
    using base::normal;
    using base::set_normal;
    ///@}

    /// Set the normal n to all the halfedges that points to vh.
    /// If you work with vertex normals, please call this function on all vertex
    /// handle before convertion with toTriangleMesh
    void propagate_normal_to_surronding_he( TopologicalMesh::VertexHandle vh );

    /// \name Const access to handles of the HalfEdge properties coming from
    /// the TriangleMesh attributes.
    ///@{
    inline const std::vector<OpenMesh::HPropHandleT<float>>& getFloatPropsHandles() const;
    inline const std::vector<OpenMesh::HPropHandleT<Vector2>>& getVector2PropsHandles() const;
    inline const std::vector<OpenMesh::HPropHandleT<Vector3>>& getVector3PropsHandles() const;
    inline const std::vector<OpenMesh::HPropHandleT<Vector4>>& getVector4PropsHandles() const;
    ///@}
};

} // namespace Core
} // namespace Ra

#include <Core/Mesh/TopologicalTriMesh/TopologicalMesh.inl>

#endif // TOPOLOGICALMESH_H
