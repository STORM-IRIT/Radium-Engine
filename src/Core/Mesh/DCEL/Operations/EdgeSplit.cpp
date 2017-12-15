#include <Core/Mesh/DCEL/Operations/EdgeSplit.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>


namespace Ra {
namespace Core {
namespace DcelOperations {

void splitEdge( Dcel& dcel, Index edgeIndex, Scalar fraction )
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


    FullEdge_ptr edge = dcel.m_fulledge[edgeIndex];


    Vertex_ptr v1 = edge->V( 0 );
    Vertex_ptr v2 = edge->V( 1 );

    // step one : create the new data structures

    // 1) The edge we will split.

    // HalfEdges from the edge we will split.
    // They will stay twin as the "left part" of the split edge.
    HalfEdge_ptr he0 = edge->HE( 0 );
    HalfEdge_ptr he1 = he0->Twin();

    // Current edge becomes the "left part"( V1->M)
    FullEdge_ptr fe0 = edge;


    // The two new half edges for the "right part"
    HalfEdge_ptr he2( new HalfEdge() );
    HalfEdge_ptr he3( new HalfEdge() );

    // New edge as the "right part" (M->V2)
    FullEdge_ptr fe1( new FullEdge( he2 ) );

    // 2) New vertex M
    Vertex_ptr vm( new Vertex(
        fraction * v1->P() + (1. - fraction) * v2->P(),
        (fraction * v1->N() + (1. - fraction) * v2->N()).normalized(),
        he2 ) );


    // 3) The two new edges created by joining M with the opposed vertices.

    // Half edge joining M to A (on side of F0)
    HalfEdge_ptr heA0( new HalfEdge );
    // Half edge joining M to A (on side of F1)
    HalfEdge_ptr heA2( new HalfEdge );
    // Full edge MA
    FullEdge_ptr feA( new FullEdge( heA0 ) );

    // Half edge joining M to B (on side of F1)
    HalfEdge_ptr heB1( new HalfEdge );
    // Half edge joining M to B (on side of F3)
    HalfEdge_ptr heB3( new HalfEdge );
    // Full edge MB
    FullEdge_ptr feB( new FullEdge( heB1 ) );

    // 4 ) Two faces adjacent to the edge to split
    Face_ptr f0 = he0->F();
    Face_ptr f1 = he1->F();

    // Two new faces
    Face_ptr f2( new Face() );
    Face_ptr f3( new Face() );


    // Step two : update the data structure

    // Save all existing points and half edges for fixup

    Vertex_ptr A = he0->Prev()->V();
    Vertex_ptr B = he1->Prev()->V();

    HalfEdge_ptr AV1 = he0->Prev();
    HalfEdge_ptr V2A = AV1->Prev();

    HalfEdge_ptr V1B = he1->Next();
    HalfEdge_ptr BV2 = V1B->Next();

    // Insert new elements
    vm->idx = dcel.m_vertex.insert( vm );

    he2->idx = dcel.m_halfedge.insert( he2 );
    he3->idx = dcel.m_halfedge.insert( he3 );

    fe1->idx = dcel.m_fulledge.insert( fe1 );

    f2->idx = dcel.m_face.insert( f2 );
    f3->idx = dcel.m_face.insert( f3 );

    // Fixup all pointers.

    he0->setNext( heA0 );
    he1->setPrev( heB1 );
    he1->setV( vm );

    he2->setV( vm );
    he2->setNext( V2A );
    he2->setPrev( heA2 );
    he2->setTwin( he3 );
    he2->setFE( fe1 );
    he2->setF( f2 );

    he3->setV( v2 );
    he3->setNext( heB3 );
    he3->setPrev( BV2 );
    he3->setTwin( he2 );
    he3->setFE( fe1 );
    he3->setF( f1 );

    heA0->setV( vm );
    heA0->setNext( AV1 );
    heA0->setPrev( he0 );
    heA0->setTwin( heA2 );
    heA0->setFE( feA );
    heA0->setF( f0 );

    heA2->setV( A );
    heA2->setNext( he2 );
    heA2->setPrev( V2A );
    heA2->setTwin( heA0 );
    heA2->setFE( feA );
    heA2->setF( f2 );

    heB1->setV( B );
    heB1->setNext( he1 );
    heB1->setPrev( V1B );
    heB1->setTwin( heB3 );
    heB1->setFE( feB );
    heB1->setF( f1 );

    heB3->setV( vm );
    heB3->setNext( BV2 );
    heB3->setPrev( he3 );
    heB3->setTwin( heB1 );
    heB3->setFE( feB );
    heB3->setF( f3 );


    if ( f0->HE() == V2A )
    {
        f0->HE() = heA0;
    }

    if ( f1->HE() == BV2 )
    {
        f1->HE() = heB1;
    }

    f2->HE() = he2;
    f3->HE() = he3;
}
}
}
}
