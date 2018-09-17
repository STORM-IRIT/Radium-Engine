#include <Core/Mesh/TopologicalTriMesh/Operations/EdgeSplit.hpp>

#include <Core/Log/Log.hpp>

namespace Ra {
namespace Core {

bool TMOperations::splitEdge( TopologicalMesh& mesh, TopologicalMesh::EdgeHandle eh, Scalar f ) {
    // Global schema of operation
    /*
               TRIANGLES ONLY
         before                after
               A                       A
            / F0 \                / F2 | F0 \
           /      \              /     |     \
          /h1    h0\            /h1  e2|e0  h0\
         /    he0   \          /  he2  |  he0  \
        V1 -------- V0       V1 ------ V ------ V0
         \    he1   /          \  he3  |  he1  /
          \o1    o0/            \o1  e3|e1  o0/
           \      /              \     |     /
            \ F1 /                \ F3 | F1 /
               B                       B

    */
    using HeHandle = TopologicalMesh::HalfedgeHandle;
    using VHandle = TopologicalMesh::VertexHandle;
    using FHandle = TopologicalMesh::FaceHandle;
    using Point = TopologicalMesh::Point;

    // incorrect factor
    if ( f < 0 || f > 1 )
    {
        return false;
    }

    // get existing topology data
    HeHandle he0 = mesh.halfedge_handle( eh, 0 );
    HeHandle he1 = mesh.halfedge_handle( eh, 1 );
    VHandle v0 = mesh.to_vertex_handle( he0 );
    VHandle v1 = mesh.to_vertex_handle( he1 );
    FHandle F0 = mesh.face_handle( he0 );
    FHandle F1 = mesh.face_handle( he1 );

    // not triangles or holes
    if ( ( !mesh.is_boundary( he0 ) && mesh.valence( F0 ) != 3 ) ||
         ( !mesh.is_boundary( he1 ) && mesh.valence( F1 ) != 3 ) )
    {
        return false;
    }

    // add the new vertex
    const Point p = Point( f * mesh.point( v0 ) + ( 1. - f ) * mesh.point( v1 ) );
    VHandle v = mesh.add_vertex( p );

    // create the new faces and reconnect the topology
    HeHandle he3 = mesh.new_edge( v, v1 );
    HeHandle he2 = mesh.opposite_halfedge_handle( he3 );
    mesh.set_halfedge_handle( v, he0 );
    mesh.set_vertex_handle( he1, v );

    // does F0 exist
    if ( !mesh.is_boundary( he0 ) )
    {
        HeHandle h0 = mesh.next_halfedge_handle( he0 );
        HeHandle h1 = mesh.next_halfedge_handle( h0 );
        // create new edge
        VHandle A = mesh.to_vertex_handle( h0 );
        HeHandle e2 = mesh.new_edge( v, A );
        HeHandle e0 = mesh.opposite_halfedge_handle( e2 );
        // split F0
        FHandle F2 = mesh.new_face();
        mesh.set_halfedge_handle( F0, he0 );
        mesh.set_halfedge_handle( F2, h1 );
        // update F0
        mesh.set_face_handle( h0, F0 );
        mesh.set_face_handle( e0, F0 );
        mesh.set_face_handle( he0, F0 );
        mesh.set_next_halfedge_handle( he0, h0 );
        mesh.set_next_halfedge_handle( h0, e0 );
        mesh.set_next_halfedge_handle( e0, he0 );
        // update F2
        mesh.set_face_handle( h1, F2 );
        mesh.set_face_handle( he2, F2 );
        mesh.set_face_handle( e2, F2 );
        mesh.set_next_halfedge_handle( e2, h1 );
        mesh.set_next_halfedge_handle( h1, he2 );
        mesh.set_next_halfedge_handle( he2, e2 );
        // deal with custom properties
        // interpolate at he2
        mesh.interpolateAllProps( h1, he0, he2, 0.5 );
        // copy at e0, and e2
        mesh.copyAllProps( he2, e0 );
    } else
    {
        HeHandle h1 = mesh.prev_halfedge_handle( he0 );
        mesh.set_next_halfedge_handle( h1, he2 );
        mesh.set_next_halfedge_handle( he2, he0 );
        // next halfedge handle of he0 already is h0
        // halfedge handle of V already is he0
    }

    // does F1 exist
    if ( !mesh.is_boundary( he1 ) )
    {
        HeHandle o1 = mesh.next_halfedge_handle( he1 );
        HeHandle o0 = mesh.next_halfedge_handle( o1 );
        // create new edge
        VHandle B = mesh.to_vertex_handle( o1 );
        HeHandle e1 = mesh.new_edge( v, B );
        HeHandle e3 = mesh.opposite_halfedge_handle( e1 );
        // split F1
        FHandle F3 = mesh.new_face();
        mesh.set_halfedge_handle( F3, o1 );
        mesh.set_halfedge_handle( F1, he1 );
        // update F1
        mesh.set_face_handle( o1, F3 );
        mesh.set_face_handle( e3, F3 );
        mesh.set_face_handle( he3, F3 );
        mesh.set_next_halfedge_handle( he3, o1 );
        mesh.set_next_halfedge_handle( o1, e3 );
        mesh.set_next_halfedge_handle( e3, he3 );
        // update F3
        mesh.set_face_handle( o0, F1 );
        mesh.set_face_handle( he1, F1 );
        mesh.set_face_handle( e1, F1 );
        mesh.set_next_halfedge_handle( he1, e1 );
        mesh.set_next_halfedge_handle( e1, o0 );
        mesh.set_next_halfedge_handle( o0, he1 );
        // deal with custom properties
        // first copy at he3
        mesh.copyAllProps( he1, he3 );
        // interpolate at he1
        mesh.interpolateAllProps( o0, he3, he1, 0.5 );
        // copy at e1, and e3
        mesh.copyAllProps( he1, e3 );
        mesh.copyAllProps( o1, e1 );
    } else
    {
        HeHandle o1 = mesh.next_halfedge_handle( he1 );
        // next halfedge handle of o0 already is he1
        mesh.set_next_halfedge_handle( he1, he3 );
        mesh.set_next_halfedge_handle( he3, o1 );
        // halfedge handle of V already is he0
    }

    // ensure consistency at v1
    if ( mesh.halfedge_handle( v1 ) == he0 )
    {
        mesh.set_halfedge_handle( v1, he2 );
    }

    return true;
}

} // namespace Core
} // namespace Ra
