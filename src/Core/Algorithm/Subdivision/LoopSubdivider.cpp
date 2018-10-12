#include <Core/Algorithm/Subdivision/LoopSubdivider.hpp>
#include <Core/Algorithm/Subdivision/SubdividerUtils.hpp>

namespace Ra {
namespace Core {

bool LoopSubdivider::prepare( TopologicalMesh& mesh ) {
    mesh.add_property( m_vpPos );
    mesh.add_property( m_epPos );
    return true;
}

bool LoopSubdivider::cleanup( TopologicalMesh& mesh ) {
    mesh.remove_property( m_vpPos );
    mesh.remove_property( m_epPos );
    return true;
}

bool LoopSubdivider::subdivide( TopologicalMesh& mesh, size_t n, const bool updatePoints ) {
    TopologicalMesh::FaceIter fit, f_end;
    TopologicalMesh::EdgeIter eit, e_end;
    TopologicalMesh::VertexIter vit;

    // Do _n subdivisions
    for ( size_t i = 0; i < n; ++i )
    {
        if ( updatePoints )
        {
            // compute new positions for old vertices
#pragma omp parallel for
            for ( uint i = 0; i < mesh.n_vertices(); ++i )
            {
                const auto& vh = mesh.vertex_handle( i );
                smooth( mesh, vh );
            }
        }

        // Compute position for new vertices and store them in the edge property
#pragma omp parallel for
        for ( uint i = 0; i < mesh.n_edges(); ++i )
        {
            const auto& eh = mesh.edge_handle( i );
            compute_midpoint( mesh, eh );
        }

        // Split each edge at midpoint and store precomputed positions (stored in
        // edge property ep_pos_) in the vertex property vp_pos_;
        // Attention! Creating new edges, hence make sure the loop ends correctly.
        e_end = mesh.edges_end();
        for ( eit = mesh.edges_begin(); eit != e_end; ++eit )
        {
            split_edge( mesh, *eit );
        }

        // Commit changes in topology and reconsitute consistency
        // Attention! Creating new faces, hence make sure the loop ends correctly.
        f_end = mesh.faces_end();
        for ( fit = mesh.faces_begin(); fit != f_end; ++fit )
        {
            split_face( mesh, *fit );
        }

        if ( updatePoints )
        {
            // Commit changes in geometry
#pragma omp parallel for
            for ( uint i = 0; i < mesh.n_vertices(); ++i )
            {
                const auto& vh = mesh.vertex_handle( i );
                mesh.set_point( vh, mesh.property( m_vpPos, vh ) );
            }
        }

        // Now we have an consistent mesh!
        CORE_ASSERT( OpenMesh::Utils::MeshCheckerT<TopologicalMesh>( mesh ).check(),
                     "LoopSubdivision ended with a bad topology." );
    }

    return true;
}

void LoopSubdivider::split_face( TopologicalMesh& mesh, const TopologicalMesh::FaceHandle& fh ) {
    using HeHandle = TopologicalMesh::HalfedgeHandle;
    // get where to cut
    HeHandle heh1( mesh.halfedge_handle( fh ) );
    HeHandle heh2( mesh.next_halfedge_handle( mesh.next_halfedge_handle( heh1 ) ) );
    HeHandle heh3( mesh.next_halfedge_handle( mesh.next_halfedge_handle( heh2 ) ) );

    // Cutting off every corner of the 6_gon
    corner_cutting( mesh, heh1 );
    corner_cutting( mesh, heh2 );
    corner_cutting( mesh, heh3 );
}

void LoopSubdivider::corner_cutting( TopologicalMesh& mesh,
                                     const TopologicalMesh::HalfedgeHandle& he ) {
    using HeHandle = TopologicalMesh::HalfedgeHandle;
    using VHandle = TopologicalMesh::VertexHandle;
    using FHandle = TopologicalMesh::FaceHandle;

    // Define Halfedge Handles
    HeHandle heh1( he );
    HeHandle heh5( heh1 );
    HeHandle heh6( mesh.next_halfedge_handle( heh1 ) );

    // Cycle around the polygon to find correct Halfedge
    for ( ; mesh.next_halfedge_handle( mesh.next_halfedge_handle( heh5 ) ) != heh1;
          heh5 = mesh.next_halfedge_handle( heh5 ) )
        ;

    VHandle vh1 = mesh.to_vertex_handle( heh1 );
    VHandle vh2 = mesh.to_vertex_handle( heh5 );

    HeHandle heh2( mesh.next_halfedge_handle( heh5 ) );
    HeHandle heh3( mesh.new_edge( vh1, vh2 ) );
    HeHandle heh4( mesh.opposite_halfedge_handle( heh3 ) );

    /* Intermediate result
     *
     *            *
     *         5 /|\
     *          /_  \
     *    vh2> *     *
     *        /|\3   |\
     *       /_  \|4   \
     *      *----\*----\*
     *          1 ^   6
     *            vh1 (adjust_outgoing halfedge!)
     */

    // Old and new Face
    FHandle fh_old( mesh.face_handle( heh6 ) );
    FHandle fh_new( mesh.new_face() );

    // Re-Set Handles around old Face
    mesh.set_next_halfedge_handle( heh4, heh6 );
    mesh.set_next_halfedge_handle( heh5, heh4 );

    mesh.set_face_handle( heh4, fh_old );
    mesh.set_face_handle( heh5, fh_old );
    mesh.set_face_handle( heh6, fh_old );
    mesh.set_halfedge_handle( fh_old, heh4 );

    // Re-Set Handles around new Face
    mesh.set_next_halfedge_handle( heh1, heh3 );
    mesh.set_next_halfedge_handle( heh3, heh2 );

    mesh.set_face_handle( heh1, fh_new );
    mesh.set_face_handle( heh2, fh_new );
    mesh.set_face_handle( heh3, fh_new );

    mesh.set_halfedge_handle( fh_new, heh1 );

    // deal with custom properties
    copyNormal( heh1, heh4, mesh );
    copyNormal( heh5, heh3, mesh );
    copyProps( mesh.getFloatPropsHandles(), heh1, heh4, mesh );
    copyProps( mesh.getFloatPropsHandles(), heh5, heh3, mesh );
    copyProps( mesh.getVector2PropsHandles(), heh1, heh4, mesh );
    copyProps( mesh.getVector2PropsHandles(), heh5, heh3, mesh );
    copyProps( mesh.getVector3PropsHandles(), heh1, heh4, mesh );
    copyProps( mesh.getVector3PropsHandles(), heh5, heh3, mesh );
    copyProps( mesh.getVector4PropsHandles(), heh1, heh4, mesh );
    copyProps( mesh.getVector4PropsHandles(), heh5, heh3, mesh );
}

void LoopSubdivider::split_edge( TopologicalMesh& mesh, const TopologicalMesh::EdgeHandle& eh ) {
    using HeHandle = TopologicalMesh::HalfedgeHandle;
    using VHandle = TopologicalMesh::VertexHandle;
    using FHandle = TopologicalMesh::FaceHandle;

    // prepare data
    HeHandle heh = mesh.halfedge_handle( eh, 0 );
    HeHandle opp_heh = mesh.halfedge_handle( eh, 1 );
    HeHandle new_heh;
    HeHandle opp_new_heh;
    HeHandle t_heh;
    VHandle vh;
    VHandle vh1( mesh.to_vertex_handle( heh ) );

    // new vertex
    vh = mesh.new_vertex( mesh.property( m_epPos, eh ) );

    // memorize position, since it will be set later
    mesh.property( m_vpPos, vh ) = mesh.property( m_epPos, eh );

    // Re-link mesh entities
    if ( mesh.is_boundary( eh ) )
    {
        for ( t_heh = heh; mesh.next_halfedge_handle( t_heh ) != opp_heh;
              t_heh = mesh.opposite_halfedge_handle( mesh.next_halfedge_handle( t_heh ) ) )
            ;
    } else
    {
        for ( t_heh = mesh.next_halfedge_handle( opp_heh );
              mesh.next_halfedge_handle( t_heh ) != opp_heh;
              t_heh = mesh.next_halfedge_handle( t_heh ) )
            ;
    }

    new_heh = mesh.new_edge( vh, vh1 );
    opp_new_heh = mesh.opposite_halfedge_handle( new_heh );
    mesh.set_vertex_handle( heh, vh );

    mesh.set_next_halfedge_handle( t_heh, opp_new_heh );
    mesh.set_next_halfedge_handle( new_heh, mesh.next_halfedge_handle( heh ) );
    mesh.set_next_halfedge_handle( heh, new_heh );
    mesh.set_next_halfedge_handle( opp_new_heh, opp_heh );

    if ( mesh.face_handle( opp_heh ).is_valid() )
    {
        mesh.set_face_handle( opp_new_heh, mesh.face_handle( opp_heh ) );
        mesh.set_halfedge_handle( mesh.face_handle( opp_new_heh ), opp_new_heh );

        // deal with custom properties
        interpolateNormal( t_heh, opp_heh, opp_new_heh, 0.5, mesh );
        interpolateProps( mesh.getFloatPropsHandles(), t_heh, opp_heh, opp_new_heh, 0.5, mesh );
        interpolateProps( mesh.getVector2PropsHandles(), t_heh, opp_heh, opp_new_heh, 0.5, mesh );
        interpolateProps( mesh.getVector3PropsHandles(), t_heh, opp_heh, opp_new_heh, 0.5, mesh );
        interpolateProps( mesh.getVector4PropsHandles(), t_heh, opp_heh, opp_new_heh, 0.5, mesh );
    }

    if ( mesh.face_handle( heh ).is_valid() )
    {
        mesh.set_face_handle( new_heh, mesh.face_handle( heh ) );
        mesh.set_halfedge_handle( mesh.face_handle( heh ), heh );

        // deal with custom properties
        copyNormal( heh, new_heh, mesh );
        copyProps( mesh.getFloatPropsHandles(), heh, new_heh, mesh );
        copyProps( mesh.getVector2PropsHandles(), heh, new_heh, mesh );
        copyProps( mesh.getVector3PropsHandles(), heh, new_heh, mesh );
        copyProps( mesh.getVector4PropsHandles(), heh, new_heh, mesh );
        HeHandle heh_prev = mesh.prev_halfedge_handle( heh );
        interpolateNormal( heh_prev, new_heh, heh, 0.5, mesh );
        interpolateProps( mesh.getFloatPropsHandles(), heh_prev, new_heh, heh, 0.5, mesh );
        interpolateProps( mesh.getVector2PropsHandles(), heh_prev, new_heh, heh, 0.5, mesh );
        interpolateProps( mesh.getVector3PropsHandles(), heh_prev, new_heh, heh, 0.5, mesh );
        interpolateProps( mesh.getVector4PropsHandles(), heh_prev, new_heh, heh, 0.5, mesh );
    }

    mesh.set_halfedge_handle( vh, new_heh );
    mesh.set_halfedge_handle( vh1, opp_new_heh );

    // Never forget this, when playing with the topology
    mesh.adjust_outgoing_halfedge( vh );
    mesh.adjust_outgoing_halfedge( vh1 );
}

void LoopSubdivider::compute_midpoint( TopologicalMesh& mesh,
                                       const TopologicalMesh::EdgeHandle& eh ) {
    TopologicalMesh::HalfedgeHandle heh = mesh.halfedge_handle( eh, 0 );
    TopologicalMesh::HalfedgeHandle opp_heh = mesh.halfedge_handle( eh, 1 );

    TopologicalMesh::Point pos = mesh.point( mesh.to_vertex_handle( heh ) );
    pos += mesh.point( mesh.to_vertex_handle( opp_heh ) );

    // boundary edge: just average vertex positions
    if ( mesh.is_boundary( eh ) )
    {
        pos *= 0.5;
    } else // inner edge: add neighbouring Vertices to sum
    {
        pos *= 3.0;
        pos += mesh.point( mesh.to_vertex_handle( mesh.next_halfedge_handle( heh ) ) );
        pos += mesh.point( mesh.to_vertex_handle( mesh.next_halfedge_handle( opp_heh ) ) );
        pos *= 1.0 / 8.0;
    }
    mesh.property( m_epPos, eh ) = pos;
}

void LoopSubdivider::smooth( TopologicalMesh& mesh, const TopologicalMesh::VertexHandle& vh ) {
    using VHandle = TopologicalMesh::VertexHandle;

    TopologicalMesh::Point pos( 0.0, 0.0, 0.0 );

    if ( mesh.is_boundary( vh ) ) // if boundary: Point 1-6-1
    {
        TopologicalMesh::HalfedgeHandle heh;
        TopologicalMesh::HalfedgeHandle prev_heh;
        heh = mesh.halfedge_handle( vh );

        if ( heh.is_valid() )
        {
            assert( mesh.is_boundary( mesh.edge_handle( heh ) ) );

            prev_heh = mesh.prev_halfedge_handle( heh );

            VHandle to_vh = mesh.to_vertex_handle( heh );
            VHandle from_vh = mesh.from_vertex_handle( prev_heh );

            // ( v_l + 6 v + v_r ) / 8
            pos = mesh.point( vh );
            pos *= 6.0;
            pos += mesh.point( to_vh );
            pos += mesh.point( from_vh );
            pos *= 1.0 / 8.0;
        } else
            return;
    } else // inner vertex: (1-a) * p + a/n * Sum q, q in one-ring of p
    {
        TopologicalMesh::VertexVertexIter vvit;
        size_t valence( 0 );

        // Calculate Valence and sum up neighbour points
        for ( vvit = mesh.cvv_iter( vh ); vvit.is_valid(); ++vvit )
        {
            ++valence;
            pos += mesh.point( *vvit );
        }
        pos *= m_weights[valence].second; // alpha(n)/n * Sum q, q in one-ring of p
        pos += m_weights[valence].first * mesh.point( vh ); // + (1-a)*p
    }

    mesh.property( m_vpPos, vh ) = pos;
}

} // namespace Core
} // namespace Ra
