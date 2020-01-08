#ifndef RADIUMENGINE_CATMULLCLARKSUBDIVIDER_H
#define RADIUMENGINE_CATMULLCLARKSUBDIVIDER_H

#include <Core/Geometry/TopologicalMesh.hpp>

#include <OpenMesh/Tools/Subdivider/Uniform/SubdividerT.hh>

namespace Ra {
namespace Core {
namespace Geometry {

/**
 * This class implements the Catmull-Clark subdivision algorithm
 *
 * This class extends OpenMesh's CatmullClarkT subdivider to handle attributes.
 * \note We here consider that boundary halfedges do not store attributes.
 */
class RA_CORE_API CatmullClarkSubdivider
    : public OpenMesh::Subdivider::Uniform::SubdividerT<TopologicalMesh, Scalar>
{

    using base   = OpenMesh::Subdivider::Uniform::SubdividerT<TopologicalMesh, Scalar>;
    using V_OP   = std::pair<Scalar, TopologicalMesh::VertexHandle>;
    using V_OPS  = std::pair<TopologicalMesh::VertexHandle, std::vector<V_OP>>;
    using SV_OPS = std::vector<V_OPS>;
    using P_OP   = std::pair<Scalar, TopologicalMesh::HalfedgeHandle>;
    using P_OPS  = std::pair<TopologicalMesh::HalfedgeHandle, std::vector<P_OP>>;
    using SP_OPS = std::vector<P_OPS>;

  public:
    CatmullClarkSubdivider() : base() {}

    explicit CatmullClarkSubdivider( TopologicalMesh& mesh ) : base() { attach( mesh ); }

    ~CatmullClarkSubdivider() { detach(); }

  public:
    const char* name( void ) const override { return "CatmullClarkSubdivider"; }

    /// In the case one wants to apply the subdivision on the same mesh topology,
    /// but with a different geometry (e.g. for an animated character),
    /// one may want to just reapply the subdivision operations instead
    /// for performance reasons.
    /// This can be achieved with the following code:
    // clang-format off
    /// \code
    /// // 1- apply subdivision once
    /// Ra::Core::Geometry::TriangleMesh triangleMesh;
    /// Ra::Core::Geometry::TopologicalMesh topoMesh( triangleMesh );
    /// Ra::Core::CatmullClarkSubdivider subdiv( topoMesh );
    /// subdiv( 2 );
    /// // get back to TriangleMesh (mandatory before re-applying)
    /// TriangleMesh subdividedMesh = topoMesh.toTriangleMesh();
    ///
    /// // 2- re-apply operations on new geometry (new_vertices, new_normals)
    /// m_subdivider.recompute( new_vertices, new_normals, subdividedMesh.vertices(),
    ///                         subdividedMesh.normals(), topoMesh );
    /// \endcode
    // clang-format on
    void recompute( const Vector3Array& newCoarseVertices,
                    const Vector3Array& newCoarseNormals,
                    Vector3Array& newSubdivVertices,
                    Vector3Array& newSubdivNormals,
                    TopologicalMesh& mesh );

  protected:
    bool prepare( TopologicalMesh& _m ) override;

    bool cleanup( TopologicalMesh& _m ) override;

    bool subdivide( TopologicalMesh& _m, size_t _n, const bool _update_points = true ) override;

  private:
    // topology helpers

    /// Edge recomposition
    void split_edge( TopologicalMesh& mesh, const TopologicalMesh::EdgeHandle& eh, size_t iter );

    /// Face recomposition
    void split_face( TopologicalMesh& mesh, const TopologicalMesh::FaceHandle& fh, size_t iter );

    // geometry helpers

    /// compute edge midpoint
    void compute_midpoint( TopologicalMesh& mesh,
                           const TopologicalMesh::EdgeHandle& eh,
                           const bool update_points,
                           size_t iter );

    /// smooth input vertices
    void
    update_vertex( TopologicalMesh& mesh, const TopologicalMesh::VertexHandle& vh, size_t iter );

  private:
    /// crease weights
    OpenMesh::EPropHandleT<Scalar> m_creaseWeights;

    /// old vertex new position
    OpenMesh::VPropHandleT<TopologicalMesh::Point> m_vpPos;

    /// new edge midpoint handle
    OpenMesh::EPropHandleT<TopologicalMesh::VertexHandle> m_epH;

    /// new face point handle
    OpenMesh::FPropHandleT<TopologicalMesh::VertexHandle> m_fpH;

    /// deal with normals on faces
    OpenMesh::FPropHandleT<TopologicalMesh::Normal> m_normalPropF;

    /// deal with custom properties on faces
    std::vector<OpenMesh::FPropHandleT<float>> m_floatPropsF;
    std::vector<OpenMesh::FPropHandleT<Vector2>> m_vec2PropsF;
    std::vector<OpenMesh::FPropHandleT<Vector3>> m_vec3PropsF;
    std::vector<OpenMesh::FPropHandleT<Vector4>> m_vec4PropsF;

    /// list of vertices computations
    std::vector<SV_OPS> m_oldVertexOps;
    std::vector<SV_OPS> m_newFaceVertexOps;
    std::vector<SV_OPS> m_newEdgeVertexOps;
    std::vector<SP_OPS> m_newEdgePropOps;
    std::vector<SP_OPS> m_newFacePropOps;
    SP_OPS m_triangulationPropOps;

    /// old vertex halfedges
    OpenMesh::HPropHandleT<TopologicalMesh::VertexHandle> m_hV;
};

} // namespace Geometry
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_CATMULLCLARKSUBDIVIDER_H
