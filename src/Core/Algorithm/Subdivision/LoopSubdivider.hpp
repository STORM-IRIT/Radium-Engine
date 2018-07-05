#ifndef RADIUMENGINE_LOOPSUBDIVIDER_H
#define RADIUMENGINE_LOOPSUBDIVIDER_H

#include <Core/Mesh/TopologicalTriMesh/TopologicalMesh.hpp>

#include <OpenMesh/Tools/Subdivider/Uniform/SubdividerT.hh>

namespace Ra {
namespace Core {

/// This class implements the Loop subdivision algorithm
///
/// This class extends OpenMesh's LoopT subdivider to handle attributes.
class RA_CORE_API LoopSubdivider
    : public OpenMesh::Subdivider::Uniform::SubdividerT<TopologicalMesh, Scalar> {
  public:
    using base = OpenMesh::Subdivider::Uniform::SubdividerT<TopologicalMesh, Scalar>;
    using Weight = std::pair<Scalar, Scalar>;
    using Weights = std::vector<Weight>;

  public:
    LoopSubdivider() : base() { init_weights(); }

    LoopSubdivider( TopologicalMesh& mesh ) : base( mesh ) { init_weights(); }

    ~LoopSubdivider() {}

  public:
    /// must implement
    const char* name( void ) const override { return "LoopSubdivider"; }

  protected:
    /// Pre-compute weights
    void init_weights( size_t max_valence = 50 ) {
        m_weights.resize( max_valence );
        std::generate( m_weights.begin(), m_weights.end(), compute_weight() );
    }

    bool prepare( TopologicalMesh& mesh ) override;

    bool cleanup( TopologicalMesh& mesh ) override;

    bool subdivide( TopologicalMesh& mesh, size_t n, const bool updatePoints = true ) override;

  private:
    /// Helper functor to compute weights for Loop-subdivision
    /// \internal
    struct compute_weight {
        compute_weight() : m_valence( -1 ) {}
        Weight operator()( void ) {
            //              1
            // alpha(n) = ---- * (40 - ( 3 + 2 cos( 2 Pi / n ) )^2 )
            //             64
            if ( ++m_valence )
            {
                double inv_v = 1.0 / double( m_valence );
                double t = ( 3.0 + 2.0 * std::cos( 2.0 * Math::Pi * inv_v ) );
                double alpha = ( 40.0 - t * t ) / 64.0;
                return Weight( 1.0 - alpha, inv_v * alpha );
            }
            return Weight( 0, 0 );
        }
        int m_valence;
    };

  private:
    // topological modifiers

    /// Face recomposition
    void split_face( TopologicalMesh& mesh, const TopologicalMesh::FaceHandle& fh );

    /// Face corner recomposition
    void corner_cutting( TopologicalMesh& mesh, const TopologicalMesh::HalfedgeHandle& he );

    /// Edge recomposition
    void split_edge( TopologicalMesh& mesh, const TopologicalMesh::EdgeHandle& eh );

    // geometry helpers

    /// compute edge midpoint
    void compute_midpoint( TopologicalMesh& mesh, const TopologicalMesh::EdgeHandle& eh );

    /// smooth input vertices
    void smooth( TopologicalMesh& mesh, const TopologicalMesh::VertexHandle& vh );

  private:
    /// old vertex new position
    OpenMesh::VPropHandleT<TopologicalMesh::Point> m_vpPos;

    /// new edge midpoint position
    OpenMesh::EPropHandleT<TopologicalMesh::Point> m_epPos;

    /// deal with properties
    std::vector<OpenMesh::HPropHandleT<float>> m_floatProps;
    std::vector<OpenMesh::HPropHandleT<Vector2>> m_vec2Props;
    std::vector<OpenMesh::HPropHandleT<Vector3>> m_vec3Props;
    std::vector<OpenMesh::HPropHandleT<Vector4>> m_vec4Props;

    /// precomputed weights
    Weights m_weights;
};

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_LOOPSUBDIVIDER_H
