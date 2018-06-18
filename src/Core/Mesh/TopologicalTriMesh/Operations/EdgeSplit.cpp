#include <Core/Mesh/TopologicalTriMesh/Operations/EdgeSplit.hpp>

#include <Core/Log/Log.hpp>

namespace Ra {
namespace Core {
namespace TMOperations {

void splitEdge( TopologicalMesh& topologicalMesh, TopologicalMesh::EdgeHandle edgeHandle,
                Scalar fraction ) {

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

    TopologicalMesh::HalfedgeHandle he0 = topologicalMesh.halfedge_handle( edgeHandle, 0 );
    TopologicalMesh::HalfedgeHandle he1 = topologicalMesh.halfedge_handle( edgeHandle, 1 );
    TopologicalMesh::VertexHandle v1 = topologicalMesh.to_vertex_handle( he0 );
    TopologicalMesh::VertexHandle v2 = topologicalMesh.to_vertex_handle( he1 );

    TopologicalMesh::Point p = TopologicalMesh::Point(
        fraction * topologicalMesh.point( v1 ) + ( 1. - fraction ) * topologicalMesh.point( v2 ) );

    //        TopologicalMesh::Normal n =  TopologicalMesh::Normal((fraction *
    //        topologicalMesh.normal(v1) + (1. - fraction) *
    //        topologicalMesh.normal(v2)).normalized());

    TopologicalMesh::VertexHandle vh = topologicalMesh.add_vertex( p );
    //        topologicalMesh.set_normal(vh,n);
    topologicalMesh.split( edgeHandle, vh );
}
} // namespace TMOperations
} // namespace Core
} // namespace Ra
