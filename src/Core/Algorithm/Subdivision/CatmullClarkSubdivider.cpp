#include <Core/Algorithm/Subdivision/CatmullClarkSubdivider.hpp>
#include <Core/Algorithm/Subdivision/SubdividerUtils.hpp>

namespace Ra {
namespace Core {

bool CatmullClarkSubdivider::prepare( TopologicalMesh& mesh ) {
    mesh.add_property( m_vpPos );
    mesh.add_property( m_epPos );
    mesh.add_property( m_fpPos );
    mesh.add_property( m_creaseWeights );
    createNormalPropOnFaces( mesh, m_normalPropF );
    createPropsOnFaces( mesh.getFloatPropsHandles(), mesh, m_floatPropsF );
    createPropsOnFaces( mesh.getVector2PropsHandles(), mesh, m_vec2PropsF );
    createPropsOnFaces( mesh.getVector3PropsHandles(), mesh, m_vec3PropsF );
    createPropsOnFaces( mesh.getVector4PropsHandles(), mesh, m_vec4PropsF );

    // initialize all weights to 0 (= smooth edge)
    for ( auto e_it = mesh.edges_begin(); e_it != mesh.edges_end(); ++e_it )
        mesh.property( m_creaseWeights, *e_it ) = 0.0;

    return true;
}

bool CatmullClarkSubdivider::cleanup( TopologicalMesh& mesh ) {
    mesh.remove_property( m_vpPos );
    mesh.remove_property( m_epPos );
    mesh.remove_property( m_fpPos );
    mesh.remove_property( m_creaseWeights );
    clearProp( m_normalPropF, mesh );
    clearProps( m_floatPropsF, mesh );
    clearProps( m_vec2PropsF, mesh );
    clearProps( m_vec3PropsF, mesh );
    clearProps( m_vec4PropsF, mesh );
    return true;
}

bool CatmullClarkSubdivider::subdivide( TopologicalMesh& mesh, size_t n,
                                        const bool update_points ) {
    // Do n subdivisions
    for ( size_t i = 0; i < n; ++i )
    {
        // Compute face centroid
#pragma omp parallel for
        for ( uint i = 0; i < mesh.n_faces(); ++i )
        {
            const auto& fh = mesh.face_handle( i );
            mesh.property( m_fpPos, fh ) = mesh.calc_face_centroid( fh );
            interpolateNormalOnFaces( m_normalPropF, fh, mesh );
            interpolatePropsOnFaces( mesh.getFloatPropsHandles(), m_floatPropsF, fh, mesh );
            interpolatePropsOnFaces( mesh.getVector2PropsHandles(), m_vec2PropsF, fh, mesh );
            interpolatePropsOnFaces( mesh.getVector3PropsHandles(), m_vec3PropsF, fh, mesh );
            interpolatePropsOnFaces( mesh.getVector4PropsHandles(), m_vec4PropsF, fh, mesh );
        }

        // Compute position for new (edge-) vertices and store them in the edge property
#pragma omp parallel for
        for ( uint i = 0; i < mesh.n_edges(); ++i )
        {
            const auto& eh = mesh.edge_handle( i );
            compute_midpoint( mesh, eh, update_points );
        }

        // position updates activated?
        if ( update_points )
        {
            // compute new positions for old vertices
#pragma omp parallel for
            for ( uint i = 0; i < mesh.n_vertices(); ++i )
            {
                const auto& vh = mesh.vertex_handle( i );
                update_vertex( mesh, vh );
            }

            // Commit changes in geometry
#pragma omp parallel for
            for ( uint i = 0; i < mesh.n_vertices(); ++i )
            {
                const auto& vh = mesh.vertex_handle( i );
                mesh.set_point( vh, mesh.property( m_vpPos, vh ) );
            }
        }

        // Split each edge at midpoint stored in edge property;
        // Attention! Creating new edges, hence make sure the loop ends correctly.
        auto e_end = mesh.edges_end();
        for ( auto e_itr = mesh.edges_begin(); e_itr != e_end; ++e_itr )
        {
            split_edge( mesh, *e_itr );
        }

        // Commit changes in topology and reconsitute consistency
        // Attention! Creating new faces, hence make sure the loop ends correctly.
        auto f_end = mesh.faces_end();
        for ( auto f_itr = mesh.faces_begin(); f_itr != f_end; ++f_itr )
        {
            split_face( mesh, *f_itr );
        }

        CORE_ASSERT( OpenMesh::Utils::MeshCheckerT<TopologicalMesh>( mesh ).check(),
                     "CatmullClarkSubdivision ended with a bad topology." );
    }

    // ###########################################################################
    // FIXME: THIS IS ONLY THERE BECAUSE CORE ONLY MANAGES TRIANGLE MESHES FOR NOW

    // triangulate resulting quads
    auto f_end = mesh.faces_end();
    for ( auto f_itr = mesh.faces_begin(); f_itr != f_end; ++f_itr )
    {
        // get all needed halfedges
        auto heh1 = mesh.halfedge_handle( *f_itr );
        auto heh2 = mesh.next_halfedge_handle( heh1 );
        auto heh3 = mesh.next_halfedge_handle( heh2 );
        auto heh4 = mesh.next_halfedge_handle( heh3 );
        auto heh5 = mesh.new_edge( mesh.to_vertex_handle( heh1 ), mesh.to_vertex_handle( heh3 ) );
        auto heh6 = mesh.opposite_halfedge_handle( heh5 );

        // triangulate
        auto fnew = mesh.new_face();
        mesh.set_halfedge_handle( fnew, heh2 );
        mesh.set_face_handle( heh5, *f_itr );
        mesh.set_face_handle( heh2, fnew );
        mesh.set_face_handle( heh3, fnew );
        mesh.set_face_handle( heh6, fnew );
        mesh.set_next_halfedge_handle( heh1, heh5 );
        mesh.set_next_halfedge_handle( heh5, heh4 );
        mesh.set_next_halfedge_handle( heh3, heh6 );
        mesh.set_next_halfedge_handle( heh6, heh2 );

        // deal with properties
        copyNormal( heh3, heh5, mesh );
        copyNormal( heh1, heh6, mesh );
        copyProps( mesh.getFloatPropsHandles(), heh3, heh5, mesh );
        copyProps( mesh.getVector2PropsHandles(), heh3, heh5, mesh );
        copyProps( mesh.getVector3PropsHandles(), heh3, heh5, mesh );
        copyProps( mesh.getVector4PropsHandles(), heh3, heh5, mesh );
        copyProps( mesh.getFloatPropsHandles(), heh1, heh6, mesh );
        copyProps( mesh.getVector2PropsHandles(), heh1, heh6, mesh );
        copyProps( mesh.getVector3PropsHandles(), heh1, heh6, mesh );
        copyProps( mesh.getVector4PropsHandles(), heh1, heh6, mesh );
    }

    return true;
}

void CatmullClarkSubdivider::split_face( TopologicalMesh& mesh,
                                         const TopologicalMesh::FaceHandle& fh ) {
    /*
        Split an n-gon into n quads by connecting
        each vertex of fh to vh.

        - fh will remain valid (it will become one of the quads)
        - the halfedge handles of the new quads will point to the old halfedges
    */

    // Since edges are already refined (valence*2)
    size_t valence = mesh.valence( fh ) / 2;

    // Add new mesh vertex from face centroid
    auto vh = mesh.add_vertex( mesh.property( m_fpPos, fh ) );

    // init new topology with first face
    auto hend = mesh.halfedge_handle( fh );
    auto hh = mesh.next_halfedge_handle( hend );
    auto hold = mesh.new_edge( mesh.to_vertex_handle( hend ), vh );
    mesh.set_next_halfedge_handle( hend, hold );
    mesh.set_face_handle( hold, fh );

    // deal with properties for vh
    copyNormal( m_normalPropF, fh, hold, mesh );
    copyProps( m_floatPropsF, mesh.getFloatPropsHandles(), fh, hold, mesh );
    copyProps( m_vec2PropsF, mesh.getVector2PropsHandles(), fh, hold, mesh );
    copyProps( m_vec3PropsF, mesh.getVector3PropsHandles(), fh, hold, mesh );
    copyProps( m_vec4PropsF, mesh.getVector4PropsHandles(), fh, hold, mesh );

    // go around new vertex to build topology
    hold = mesh.opposite_halfedge_handle( hold );

    // deal with properties for hold
    copyNormal( hend, hold, mesh );
    copyProps( mesh.getFloatPropsHandles(), hend, hold, mesh );
    copyProps( mesh.getVector2PropsHandles(), hend, hold, mesh );
    copyProps( mesh.getVector3PropsHandles(), hend, hold, mesh );
    copyProps( mesh.getVector4PropsHandles(), hend, hold, mesh );

    for ( size_t i = 1; i < valence; i++ )
    {
        // go to next mid-edge vertex
        auto hnext = mesh.next_halfedge_handle( hh );
        auto hnew = mesh.new_edge( mesh.to_vertex_handle( hnext ), vh );

        // create new face
        auto fnew = mesh.new_face();
        mesh.set_halfedge_handle( fnew, hh );
        mesh.set_face_handle( hnew, fnew );
        mesh.set_face_handle( hold, fnew );
        mesh.set_face_handle( hh, fnew );
        mesh.set_face_handle( hnext, fnew );
        mesh.set_next_halfedge_handle( hnew, hold );
        mesh.set_next_halfedge_handle( hold, hh );

        // deal with properties for hnew
        copyNormal( m_normalPropF, fh, hnew, mesh );
        copyProps( m_floatPropsF, mesh.getFloatPropsHandles(), fh, hnew, mesh );
        copyProps( m_vec2PropsF, mesh.getVector2PropsHandles(), fh, hnew, mesh );
        copyProps( m_vec3PropsF, mesh.getVector3PropsHandles(), fh, hnew, mesh );
        copyProps( m_vec4PropsF, mesh.getVector4PropsHandles(), fh, hnew, mesh );

        // prepare for next face
        hh = mesh.next_halfedge_handle( hnext );
        hold = mesh.opposite_halfedge_handle( hnew );
        mesh.set_next_halfedge_handle( hnext, hnew ); // this has to be done after hh !

        // deal with properties for hold
        copyNormal( hnext, hold, mesh );
        copyProps( mesh.getFloatPropsHandles(), hnext, hold, mesh );
        copyProps( mesh.getVector2PropsHandles(), hnext, hold, mesh );
        copyProps( mesh.getVector3PropsHandles(), hnext, hold, mesh );
        copyProps( mesh.getVector4PropsHandles(), hnext, hold, mesh );
    }

    // finish topology
    mesh.set_next_halfedge_handle( hold, hh );
    mesh.set_next_halfedge_handle( hh, hend );
    hh = mesh.next_halfedge_handle( hend );
    mesh.set_next_halfedge_handle( hh, hold );
    mesh.set_face_handle( hold, fh );
    mesh.set_halfedge_handle( vh, hold );
}

void CatmullClarkSubdivider::split_edge( TopologicalMesh& mesh,
                                         const TopologicalMesh::EdgeHandle& eh ) {
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

    // Re-link mesh entities
    if ( mesh.is_boundary( eh ) )
    {
        // spin around vh1
        for ( t_heh = heh; mesh.next_halfedge_handle( t_heh ) != opp_heh;
              t_heh = mesh.opposite_halfedge_handle( mesh.next_halfedge_handle( t_heh ) ) )
            ;
    } else
    {
        // spin inside opp_heh's face
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

void CatmullClarkSubdivider::compute_midpoint( TopologicalMesh& mesh,
                                               const TopologicalMesh::EdgeHandle& eh,
                                               const bool update_points ) {
    TopologicalMesh::HalfedgeHandle heh = mesh.halfedge_handle( eh, 0 );
    TopologicalMesh::HalfedgeHandle opp_heh = mesh.halfedge_handle( eh, 1 );

    TopologicalMesh::Point pos = mesh.point( mesh.to_vertex_handle( heh ) );
    pos += mesh.point( mesh.to_vertex_handle( opp_heh ) );

    // boundary edge: just average vertex positions
    // this yields the [1/2 1/2] mask
    if ( mesh.is_boundary( eh ) || !update_points )
    {
        pos *= 0.5;
    } else // inner edge: add neighbouring Vertices to sum
           // this yields the [1/16 1/16; 3/8 3/8; 1/16 1/16] mask
    {
        pos += mesh.property( m_fpPos, mesh.face_handle( heh ) );
        pos += mesh.property( m_fpPos, mesh.face_handle( opp_heh ) );
        pos *= 0.25;
    }
    mesh.property( m_epPos, eh ) = pos;
}

void CatmullClarkSubdivider::update_vertex( TopologicalMesh& mesh,
                                            const TopologicalMesh::VertexHandle& vh ) {
    TopologicalMesh::Point pos( 0.0, 0.0, 0.0 );

    // TODO boundary, Extraordinary Vertex and  Creased Surfaces
    // see "A Factored Approach to Subdivision Surfaces"
    // http://faculty.cs.tamu.edu/schaefer/research/tutorial.pdf
    // and http://www.cs.utah.edu/~lacewell/subdeval
    if ( mesh.is_boundary( vh ) )
    {
        pos = mesh.point( vh );
        for ( auto ve_itr = mesh.cve_iter( vh ); ve_itr.is_valid(); ++ve_itr )
            if ( mesh.is_boundary( *ve_itr ) )
                pos += mesh.property( m_epPos, *ve_itr );
        pos /= 3.0;
    } else // inner vertex
    {
        /* For each (non boundary) vertex V, introduce a new vertex whose
           position is F/n + 2E/n + (n-3)V/n where F is the average of
           the new face vertices of all faces adjacent to the old vertex
           V, E is the average of the midpoints of all edges incident
           on the old vertex V, and n is the number of edges incident on
           the vertex.
       */
        Scalar valence = 0.0;
        for ( auto voh_it = mesh.cvoh_iter( vh ); voh_it.is_valid(); ++voh_it )
        {
            pos += mesh.point( mesh.to_vertex_handle( *voh_it ) );
            valence += 1.0;
        }
        pos /= valence * valence;

        TopologicalMesh::Point Q( 0, 0, 0 );

        for ( auto vf_itr = mesh.cvf_iter( vh ); vf_itr.is_valid(); ++vf_itr )
        {
            Q += mesh.property( m_fpPos, *vf_itr );
        }

        Q /= valence * valence;

        pos += mesh.point( vh ) * ( valence - 2.0 ) / valence + Q;
    }

    mesh.property( m_vpPos, vh ) = pos;
}

} // namespace Core
} // namespace Ra
