
#include <Core/Mesh/TopologicalTriMesh/TopologicalMesh.hpp>

namespace Ra {
namespace Core {

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

/// Add a copy of \p input properties of \p mesh on faces.
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

/// Remove \props from \p mesh.
template <typename T>
void clearProps( std::vector<OpenMesh::HPropHandleT<T>>& props, TopologicalMesh& mesh ) {
    for ( auto& oh : props )
    {
        mesh.remove_property( oh );
    }
    props.clear();
}

/// Remove \props from \p mesh.
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
        mesh.property( oh, out ) = f * ( mesh.property( oh, in_a ) + mesh.property( oh, in_b ) );
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
