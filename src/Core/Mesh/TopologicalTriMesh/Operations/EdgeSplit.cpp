#include <Core/Mesh/TopologicalTriMesh/Operations/EdgeSplit.hpp>

#include <Core/Log/Log.hpp>

namespace Ra {
namespace Core {
namespace TMOperations {

void splitEdge( TopologicalMesh& mesh, TopologicalMesh::EdgeHandle edgeHandle, Scalar fraction ) {
    // Global schema of operation
    /*

     before                                          after

          A                                            A
        /   \                                     /    |    \
       /  F0 \                                   / F0  |  F2 \
      /       \                                 /      |      \
     / --he0-> \                               / he0-> | he2-> \
    V1--edge --V2                           V1  -----  M ------V2
     \ <-he1-- /                               \ <-he1 | <-he3 /
      \       /                                 \      |      /
       \  F1 /                                   \ F1  |  F3 /
        \   /                                     \    |    /
          B                                        \   B   /


    */

    CORE_ASSERT( fraction > 0 && fraction < 1, "Invalid fraction" );

    TopologicalMesh::HalfedgeHandle he0 = mesh.halfedge_handle( edgeHandle, 0 );
    TopologicalMesh::HalfedgeHandle he1 = mesh.halfedge_handle( edgeHandle, 1 );
    TopologicalMesh::VertexHandle v1 = mesh.to_vertex_handle( he0 );
    TopologicalMesh::VertexHandle v2 = mesh.to_vertex_handle( he1 );

    TopologicalMesh::Point p = TopologicalMesh::Point( fraction * mesh.point( v1 ) +
                                                       ( 1. - fraction ) * mesh.point( v2 ) );

    // TopologicalMesh::Normal n =  TopologicalMesh::Normal(
    //     ( fraction * mesh.normal( v1 ) + ( 1. - fraction ) * mesh.normal( v2 ) ).normalized() );

    TopologicalMesh::VertexHandle vh = mesh.add_vertex( p );
    // mesh.set_normal(vh,n);
    mesh.split( edgeHandle, vh );
}

} // namespace TMOperations
} // namespace Core
} // namespace Ra
