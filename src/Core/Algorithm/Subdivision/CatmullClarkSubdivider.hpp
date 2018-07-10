#ifndef RADIUMENGINE_CATMULLCLARKSUBDIVIDER_H
#define RADIUMENGINE_CATMULLCLARKSUBDIVIDER_H

#include <Core/Mesh/TopologicalTriMesh/TopologicalMesh.hpp>

#include <OpenMesh/Tools/Subdivider/Uniform/SubdividerT.hh>

namespace Ra {
namespace Core {

/// This class implements the Loop subdivision algorithm
///
/// This class extends OpenMesh's CatmullClarkT subdivider to handle attributes.
class RA_CORE_API CatmullClarkSubdivider
    : public OpenMesh::Subdivider::Uniform::SubdividerT<TopologicalMesh, Scalar> {
  public:
    using base = OpenMesh::Subdivider::Uniform::SubdividerT<TopologicalMesh, Scalar>;

  public:
    CatmullClarkSubdivider() : base() {}

    CatmullClarkSubdivider( TopologicalMesh& mesh ) : base( mesh ) {}

    ~CatmullClarkSubdivider() {}

  public:
    /// must implement
    const char* name( void ) const override { return "CatmullClarkSubdivider"; }

  protected:
    virtual bool prepare( TopologicalMesh& _m ) override;

    virtual bool cleanup( TopologicalMesh& _m ) override;

    virtual bool subdivide( TopologicalMesh& _m, size_t _n,
                            const bool _update_points = true ) override;

  private:
    // topology helpers

    void split_edge( TopologicalMesh& mesh, const TopologicalMesh::EdgeHandle& eh );

    void split_face( TopologicalMesh& mesh, const TopologicalMesh::FaceHandle& fh );

    void compute_midpoint( TopologicalMesh& mesh, const TopologicalMesh::EdgeHandle& eh,
                           const bool update_points );

    void update_vertex( TopologicalMesh& mesh, const TopologicalMesh::VertexHandle& vh );

  private:
    /// old vertex new position
    OpenMesh::VPropHandleT<TopologicalMesh::Point> m_vpPos;

    /// new edge midpoint position
    OpenMesh::EPropHandleT<TopologicalMesh::Point> m_epPos;

    /// new face pts
    OpenMesh::FPropHandleT<TopologicalMesh::Point> m_fpPos;

    /// crease weights
    OpenMesh::EPropHandleT<Scalar> m_creaseWeights;

    /// deal with properties
    std::vector<OpenMesh::HPropHandleT<float>> m_floatProps;
    std::vector<OpenMesh::HPropHandleT<Vector2>> m_vec2Props;
    std::vector<OpenMesh::HPropHandleT<Vector3>> m_vec3Props;
    std::vector<OpenMesh::HPropHandleT<Vector4>> m_vec4Props;
    std::vector<OpenMesh::FPropHandleT<float>> m_floatPropsF;
    std::vector<OpenMesh::FPropHandleT<Vector2>> m_vec2PropsF;
    std::vector<OpenMesh::FPropHandleT<Vector3>> m_vec3PropsF;
    std::vector<OpenMesh::FPropHandleT<Vector4>> m_vec4PropsF;
};

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_LOOPSUBDIVIDER_H
