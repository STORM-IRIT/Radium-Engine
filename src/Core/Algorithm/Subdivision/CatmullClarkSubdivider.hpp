#ifndef RADIUMENGINE_CATMULLCLARKSUBDIVIDER_H
#define RADIUMENGINE_CATMULLCLARKSUBDIVIDER_H

#include <Core/Mesh/TopologicalTriMesh/TopologicalMesh.hpp>

#include <OpenMesh/Tools/Subdivider/Uniform/SubdividerT.hh>

namespace Ra {
namespace Core {

/// This class implements the Catmull-Clark subdivision algorithm
///
/// This class extends OpenMesh's CatmullClarkT subdivider to handle attributes.
/// \note We here consider that boundary halfedges do not store attributes.
class RA_CORE_API CatmullClarkSubdivider
    : public OpenMesh::Subdivider::Uniform::SubdividerT<TopologicalMesh, Scalar> {
  public:
    using base = OpenMesh::Subdivider::Uniform::SubdividerT<TopologicalMesh, Scalar>;

  public:
    CatmullClarkSubdivider() : base() {}

    CatmullClarkSubdivider( TopologicalMesh& mesh ) : base() { attach( mesh ); }

    ~CatmullClarkSubdivider() {}

  public:
    const char* name( void ) const override { return "CatmullClarkSubdivider"; }

  protected:
    bool prepare( TopologicalMesh& _m ) override;

    bool cleanup( TopologicalMesh& _m ) override;

    bool subdivide( TopologicalMesh& _m, size_t _n, const bool _update_points = true ) override;

  private:
    // topology helpers

    /// Edge recomposition
    void split_edge( TopologicalMesh& mesh, const TopologicalMesh::EdgeHandle& eh );

    /// Face recomposition
    void split_face( TopologicalMesh& mesh, const TopologicalMesh::FaceHandle& fh );

    // geometry helpers

    /// compute edge midpoint
    void compute_midpoint( TopologicalMesh& mesh, const TopologicalMesh::EdgeHandle& eh,
                           const bool update_points );

    /// smooth input vertices
    void update_vertex( TopologicalMesh& mesh, const TopologicalMesh::VertexHandle& vh );

  private:
    /// crease weights
    OpenMesh::EPropHandleT<Scalar> m_creaseWeights;

    /// old vertex new position
    OpenMesh::VPropHandleT<TopologicalMesh::Point> m_vpPos;

    /// new edge midpoint position
    OpenMesh::EPropHandleT<TopologicalMesh::Point> m_epPos;

    /// new face point position
    OpenMesh::FPropHandleT<TopologicalMesh::Point> m_fpPos;

    /// deal with normals on faces
    OpenMesh::FPropHandleT<TopologicalMesh::Normal> m_normalPropF;

    /// deal with custom properties on faces
    std::vector<OpenMesh::FPropHandleT<float>> m_floatPropsF;
    std::vector<OpenMesh::FPropHandleT<Vector2>> m_vec2PropsF;
    std::vector<OpenMesh::FPropHandleT<Vector3>> m_vec3PropsF;
    std::vector<OpenMesh::FPropHandleT<Vector4>> m_vec4PropsF;
};

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_CATMULLCLARKSUBDIVIDER_H
