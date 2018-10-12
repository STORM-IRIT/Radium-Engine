
#include <Core/Mesh/TopologicalTriMesh/TopologicalMesh.hpp>

namespace Ra {
namespace Core {

// Deal with normals

/// Create a new property for normals of \p mesh on faces.
inline void createNormalPropOnFaces( TopologicalMesh& mesh,
                                     OpenMesh::FPropHandleT<TopologicalMesh::Normal>& copy ) {
    auto nph = mesh.halfedge_normals_pph();
    mesh.add_property( copy, mesh.property( nph ).name() + "_subdiv_copy_F" );
}

/// Remove face property \p prop from \p mesh.
inline void clearProp( OpenMesh::FPropHandleT<TopologicalMesh::Normal>& prop,
                       TopologicalMesh& mesh ) {
    mesh.remove_property( prop );
    prop = OpenMesh::FPropHandleT<TopologicalMesh::Normal>();
}

/// Copy \p props property from \p input_heh to \p copy_heh
inline void copyNormal( const TopologicalMesh::HalfedgeHandle& input_heh,
                        const TopologicalMesh::HalfedgeHandle& copy_heh, TopologicalMesh& mesh ) {
    auto nph = mesh.halfedge_normals_pph();
    mesh.property( nph, copy_heh ) = mesh.property( nph, input_heh );
}

/// Copy \p fprops properties from \p input_fh to \p copy_heh
inline void copyNormal( const OpenMesh::FPropHandleT<TopologicalMesh::Normal>& fprop,
                        const TopologicalMesh::FaceHandle& input_fh,
                        const TopologicalMesh::HalfedgeHandle& copy_heh, TopologicalMesh& mesh ) {
    auto nph = mesh.halfedge_normals_pph();
    mesh.property( nph, copy_heh ) = mesh.property( fprop, input_fh );
}

/// Interpolate \p prop on edge center (after edge split).
inline void interpolateNormal( const TopologicalMesh::HalfedgeHandle& in_a,
                               const TopologicalMesh::HalfedgeHandle& in_b,
                               const TopologicalMesh::HalfedgeHandle& out, Scalar f,
                               TopologicalMesh& mesh ) {
    auto nph = mesh.halfedge_normals_pph();
    mesh.property( nph, out ) =
        ( ( 1 - f ) * mesh.property( nph, in_a ) + f * mesh.property( nph, in_b ) ).normalized();
}

/// Interpolate \p hprop on face center.
inline void interpolateNormalOnFaces( const OpenMesh::FPropHandleT<TopologicalMesh::Normal>& fprop,
                                      const TopologicalMesh::FaceHandle& fh,
                                      TopologicalMesh& mesh ) {
    auto nph = mesh.halfedge_normals_pph();

    // init sum to first
    auto heh = mesh.halfedge_handle( fh );
    mesh.property( fprop, fh ) = mesh.property( nph, heh );
    heh = mesh.next_halfedge_handle( heh );

    // sum others
    for ( int i = 1; i < mesh.valence( fh ); ++i )
    {
        mesh.property( fprop, fh ) += mesh.property( nph, heh );
        heh = mesh.next_halfedge_handle( heh );
    }

    // normalize
    mesh.property( fprop, fh ) = mesh.property( fprop, fh ).normalized();
}

// Deal with custom properties

/// Create a new property for each \p input properties of \p mesh on faces.
template <typename T>
void createPropsOnFaces( const std::vector<OpenMesh::HPropHandleT<T>>& input, TopologicalMesh& mesh,
                         std::vector<OpenMesh::FPropHandleT<T>>& output ) {
    output.reserve( input.size() );
    for ( const auto& oh : input )
    {
        OpenMesh::FPropHandleT<T> oh_;
        mesh.add_property( oh_, mesh.property( oh ).name() + "_subdiv_copy_F" );
        output.push_back( oh_ );
    }
}

/// Remove \p props from \p mesh.
template <typename T>
void clearProps( std::vector<OpenMesh::FPropHandleT<T>>& props, TopologicalMesh& mesh ) {
    for ( auto& oh : props )
    {
        mesh.remove_property( oh );
    }
    props.clear();
}

/// Copy \p props properties from \p input_heh to \p copy_heh
template <typename T>
void copyProps( const std::vector<OpenMesh::HPropHandleT<T>>& props,
                const TopologicalMesh::HalfedgeHandle& input_heh,
                const TopologicalMesh::HalfedgeHandle& copy_heh, TopologicalMesh& mesh ) {
    for ( const auto& oh : props )
    {
        mesh.property( oh, copy_heh ) = mesh.property( oh, input_heh );
    }
}

/// Copy \p fprops properties from \p input_fh to \p copy_heh
template <typename T>
void copyProps( const std::vector<OpenMesh::FPropHandleT<T>>& fprops,
                const std::vector<OpenMesh::HPropHandleT<T>>& hprops,
                const TopologicalMesh::FaceHandle& input_fh,
                const TopologicalMesh::HalfedgeHandle& copy_heh, TopologicalMesh& mesh ) {
    for ( uint i = 0; i < hprops.size(); ++i )
    {
        auto hp = hprops[i];
        auto fp = fprops[i];
        mesh.property( hp, copy_heh ) = mesh.property( fp, input_fh );
    }
}

/// Interpolate \p props on edge center (after edge split).
template <typename T>
void interpolateProps( const std::vector<OpenMesh::HPropHandleT<T>>& props,
                       const TopologicalMesh::HalfedgeHandle& in_a,
                       const TopologicalMesh::HalfedgeHandle& in_b,
                       const TopologicalMesh::HalfedgeHandle& out, Scalar f,
                       TopologicalMesh& mesh ) {
    // interpolate properties
    for ( const auto& oh : props )
    {
        mesh.property( oh, out ) =
            ( 1 - f ) * mesh.property( oh, in_a ) + f * mesh.property( oh, in_b );
    }
}

/// Interpolate \p hprops on face center.
template <typename T>
void interpolatePropsOnFaces( const std::vector<OpenMesh::HPropHandleT<T>>& hprops,
                              const std::vector<OpenMesh::FPropHandleT<T>>& fprops,
                              const TopologicalMesh::FaceHandle& fh, TopologicalMesh& mesh ) {
    auto heh = mesh.halfedge_handle( fh );
    const int valence = mesh.valence( fh );

    // init sum to first
    for ( int j = 0; j < fprops.size(); ++j )
    {
        auto hp = hprops[j];
        auto fp = fprops[j];
        mesh.property( fp, fh ) = mesh.property( hp, heh );
    }
    heh = mesh.next_halfedge_handle( heh );
    // sum others
    for ( int i = 1; i < valence; ++i )
    {
        for ( int j = 0; j < hprops.size(); ++j )
        {
            auto hp = hprops[j];
            auto fp = fprops[j];
            mesh.property( fp, fh ) += mesh.property( hp, heh );
        }
        heh = mesh.next_halfedge_handle( heh );
    }
    // normalize
    for ( int j = 0; j < fprops.size(); ++j )
    {
        auto fp = fprops[j];
        mesh.property( fp, fh ) = mesh.property( fp, fh ) / valence;
    }
}

} // namespace Core
} // namespace Ra
