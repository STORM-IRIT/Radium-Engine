
#include <Core/Mesh/TopologicalTriMesh/TopologicalMesh.hpp>

namespace Ra {
namespace Core {

// Deal with normals

/// Copy normal \p input property of mesh.
inline void addPropCopy( const OpenMesh::HPropHandleT<TopologicalMesh::Normal>& input,
                         TopologicalMesh& mesh,
                         OpenMesh::HPropHandleT<TopologicalMesh::Normal>& copy ) {
    // add the new property
    mesh.add_property( copy, mesh.property( input ).name() + "_subdiv_copy" );
    // copy values
#pragma omp parallel for
    for ( int i = 0; i < mesh.n_halfedges(); ++i )
    {
        auto heh = mesh.halfedge_handle( i );
        mesh.property( copy, heh ) = mesh.property( input, heh );
    }
}

/// Create a new property for \p input property of \p mesh on faces.
inline void addPropCopyF( const OpenMesh::HPropHandleT<TopologicalMesh::Normal>& input,
                          TopologicalMesh& mesh,
                          OpenMesh::FPropHandleT<TopologicalMesh::Normal>& copy ) {
    // add the new property
    mesh.add_property( copy, mesh.property( input ).name() + "_subdiv_copy_F" );
}

/// Remove \p prop from \p mesh.
inline void clearProp( OpenMesh::HPropHandleT<TopologicalMesh::Normal>& prop,
                       TopologicalMesh& mesh ) {
    mesh.remove_property( prop );
    prop = OpenMesh::HPropHandleT<TopologicalMesh::Normal>();
}

/// Remove \p props from \p mesh.
inline void clearProp( OpenMesh::FPropHandleT<TopologicalMesh::Normal>& prop,
                       TopologicalMesh& mesh ) {
    mesh.remove_property( prop );
    prop = OpenMesh::FPropHandleT<TopologicalMesh::Normal>();
}

/// Update \p copy property from input.
inline void commitProp( const OpenMesh::HPropHandleT<TopologicalMesh::Normal>& input,
                        TopologicalMesh& mesh,
                        const OpenMesh::HPropHandleT<TopologicalMesh::Normal>& copy ) {
    // copy values
#pragma omp parallel for
    for ( int i = 0; i < mesh.n_halfedges(); ++i )
    {
        auto heh = mesh.halfedge_handle( i );
        mesh.property( copy, heh ) = mesh.property( input, heh );
    }
}

/// Copy \p props property from \p input_heh to \p copy_heh
inline void copyProp( const OpenMesh::HPropHandleT<TopologicalMesh::Normal>& prop,
                      const TopologicalMesh::HalfedgeHandle& input_heh,
                      const TopologicalMesh::HalfedgeHandle& copy_heh, TopologicalMesh& mesh ) {
    mesh.property( prop, copy_heh ) = mesh.property( prop, input_heh );
}

/// Copy \p fprops properties from \p input_fh to \p copy_heh
inline void copyProp( const OpenMesh::FPropHandleT<TopologicalMesh::Normal>& fprop,
                      const OpenMesh::HPropHandleT<TopologicalMesh::Normal>& hprop,
                      const TopologicalMesh::FaceHandle& input_fh,
                      const TopologicalMesh::HalfedgeHandle& copy_heh, TopologicalMesh& mesh ) {
    mesh.property( hprop, copy_heh ) = mesh.property( fprop, input_fh );
}

/// Interpolate \p prop on edge center (after edge split).
inline void interpolateProp( const OpenMesh::HPropHandleT<TopologicalMesh::Normal>& prop,
                             const TopologicalMesh::HalfedgeHandle& in_a,
                             const TopologicalMesh::HalfedgeHandle& in_b,
                             const TopologicalMesh::HalfedgeHandle& out, Scalar f,
                             TopologicalMesh& mesh ) {
    mesh.property( prop, out ) =
        ( ( 1 - f ) * mesh.property( prop, in_a ) + f * mesh.property( prop, in_b ) ).normalized();
}

/// Interpolate \p hprop on face center.
inline void interpolateProp( const OpenMesh::HPropHandleT<TopologicalMesh::Normal>& hprop,
                             const OpenMesh::FPropHandleT<TopologicalMesh::Normal>& fprop,
                             const TopologicalMesh::FaceHandle& fh, TopologicalMesh& mesh ) {
    auto heh = mesh.halfedge_handle( fh );
    const int valence = mesh.valence( fh );

    // init sum to first
    mesh.property( fprop, fh ) = mesh.property( hprop, heh );
    heh = mesh.next_halfedge_handle( heh );
    // sum others
    for ( int i = 1; i < valence; ++i )
    {
        mesh.property( fprop, fh ) += mesh.property( hprop, heh );
        heh = mesh.next_halfedge_handle( heh );
    }
    // normalize
    mesh.property( fprop, fh ) = mesh.property( fprop, fh ) / valence;
}

// Deal with custom properties

/// Copy \p input properties of \p mesh.
template <typename T>
void addPropsCopy( const std::vector<OpenMesh::HPropHandleT<T>>& input, TopologicalMesh& mesh,
                   std::vector<OpenMesh::HPropHandleT<T>>& copy ) {
    copy.reserve( input.size() );
    for ( const auto& oh : input )
    {
        // make new property
        OpenMesh::HPropHandleT<T> oh_;
        mesh.add_property( oh_, mesh.property( oh ).name() + "_subdiv_copy" );
        copy.push_back( oh_ );
        // copy values
#pragma omp parallel for
        for ( int i = 0; i < mesh.n_halfedges(); ++i )
        {
            auto heh = mesh.halfedge_handle( i );
            mesh.property( oh_, heh ) = mesh.property( oh, heh );
        }
    }
}

/// Create a new property for each \p input properties of \p mesh on faces.
template <typename T>
void addPropsCopyF( const std::vector<OpenMesh::HPropHandleT<T>>& input, TopologicalMesh& mesh,
                    std::vector<OpenMesh::FPropHandleT<T>>& copy ) {
    copy.reserve( input.size() );
    for ( const auto& oh : input )
    {
        OpenMesh::FPropHandleT<T> oh_;
        mesh.add_property( oh_, mesh.property( oh ).name() + "_subdiv_copy_F" );
        copy.push_back( oh_ );
    }
}

/// Remove \p props from \p mesh.
template <typename T>
void clearProps( std::vector<OpenMesh::HPropHandleT<T>>& props, TopologicalMesh& mesh ) {
    for ( auto& oh : props )
    {
        mesh.remove_property( oh );
    }
    props.clear();
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

/// Update \p copy properties from input.
template <typename T>
void commitProps( const std::vector<OpenMesh::HPropHandleT<T>>& input, TopologicalMesh& mesh,
                  const std::vector<OpenMesh::HPropHandleT<T>>& copy ) {
    for ( uint i = 0; i < input.size(); ++i )
    {
        auto oh = input[i];
        auto oh_ = copy[i];
        // copy values
#pragma omp parallel for
        for ( int i = 0; i < mesh.n_halfedges(); ++i )
        {
            auto heh = mesh.halfedge_handle( i );
            mesh.property( oh_, heh ) = mesh.property( oh, heh );
        }
    }
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
void interpolateProps( const std::vector<OpenMesh::HPropHandleT<T>>& hprops,
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
