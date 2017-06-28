#include <Core/Mesh/TopologicalTriMesh/Operations/EdgeSplit.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>

#include <Core/Log/Log.hpp>


namespace Ra {
namespace Core {
namespace TTMOperations {

    void splitEdge( TopologicalMesh& topoMesh, Index edgeIndex, Scalar fraction )
    {

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

        TopologicalMesh::EdgeHandle eh = topoMesh.edge_handle(edgeIndex);
        TopologicalMesh::VertexHandle v1 = topoMesh.to_vertex_handle(topoMesh.halfedge_handle(eh,0));
        TopologicalMesh::VertexHandle v2 = topoMesh.to_vertex_handle(topoMesh.halfedge_handle(eh,1));

        TopologicalMesh::Point p =  TopologicalMesh::Point(fraction * topoMesh.point(v1) + (1. - fraction) * topoMesh.point(v2));
        TopologicalMesh::Normal n =  TopologicalMesh::Normal((fraction * topoMesh.normal(v1) + (1. - fraction) * topoMesh.normal(v2)).normalized());

        TopologicalMesh::VertexHandle vh = topoMesh.add_vertex(p);
        topoMesh.set_normal(vh,n);
        topoMesh.split(eh,vh);
        topoMesh.garbage_collection();

    }
}
}
}
