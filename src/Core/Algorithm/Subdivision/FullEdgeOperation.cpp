#include <Core/Algorithm/Subdivision/FullEdgeOperation.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>
#include <Core/Mesh/DCEL/Dcel.hpp>

namespace Ra {
namespace Core {

void fulledgeSplit( Dcel& dcel, const Index fulledge_id ) {
    FullEdge_ptr ptr;
    if( dcel.m_fulledge.access( fulledge_id, ptr ) ) {
        // Declare the data
        Vertex_ptr   v;
        HalfEdge_ptr he[4][3];
        FullEdge_ptr fe[4];
        Face_ptr     f[4];

        // Create the new vertex
        v = std::shared_ptr< Vertex >( new Vertex( ( 0.5 * ( ptr->V( 0 )->P() + ptr->V( 1 )->P() ) ),
                                                   ( ( ptr->V( 0 )->N() + ptr->V( 1 )->N() ).normalized() ),
                                                   ptr->HE( 1 ) ) );

        // Prepare the halfedges
        he[0][0] = ptr->HE( 0 )->Prev();
        he[0][1] = ptr->HE( 0 );
        he[0][2] = std::shared_ptr<  HalfEdge >( new HalfEdge( v ) );
        he[1][0] = ptr->HE( 1 )->Next();
        he[1][1] = std::shared_ptr< HalfEdge >( new HalfEdge( ptr->HE( 1 )->Prev()->V() ) );
        he[1][2] = ptr->HE( 1 );
        he[2][0] = ptr->HE( 1 )->Prev();
        he[2][1] = std::shared_ptr< HalfEdge >( new HalfEdge( ptr->HE( 1 )->V() ) );
        he[2][2] = std::shared_ptr< HalfEdge >( new HalfEdge( v ) );
        he[3][0] = ptr->HE( 0 )->Next();
        he[3][1] = std::shared_ptr< HalfEdge >( new HalfEdge( ptr->HE( 0 )->Prev()->V() ) );
        he[3][2] = std::shared_ptr< HalfEdge >( new HalfEdge( v ) );

        // Prepare the fulledges
        fe[0] = ptr;
        fe[1] = std::shared_ptr< FullEdge >( new FullEdge( he[1][1] ) );
        fe[2] = std::shared_ptr< FullEdge >( new FullEdge( he[2][1] ) );
        fe[3] = std::shared_ptr< FullEdge >( new FullEdge( he[3][1] ) );

        // Prepare the faces
        f[0] = he[0][0]->F();
        f[1] = he[1][0]->F();
        f[2] = std::shared_ptr< Face >( new Face( he[2][0] ) );
        f[3] = std::shared_ptr< Face >( new Face( he[3][0] ) );

        // Set the data
        he[1][2]->setV( v );
        for( uint i = 0; i < 4; ++i ) {
            f[i]->setHE( he[i][0] );
            for( uint j = 0; j < 3; ++j ) {
                he[i][j]->setNext( he[i][( j + 1 ) % 3] );
                he[i][j]->setPrev( he[i][( j + 2 ) % 3] );
                he[i][j]->setFE( fe[i] );
                he[i][j]->setF( f[i] );
            }
            he[i][1]->setTwin( he[( i + 1 ) % 4][2] );
            he[i][2]->setTwin( he[( i + 3 ) % 4][1] );
        }

        // Insert new data
        CORE_ASSERT( dcel.m_vertex.insert( v, v->idx ), "New vertex not inserted" );
        CORE_ASSERT( dcel.m_halfedge.insert( he[0][2], he[0][2]->idx ), "New halfedge[0][2] not inserted" );
        CORE_ASSERT( dcel.m_halfedge.insert( he[1][1], he[1][1]->idx ), "New halfedge[1][1] not inserted" );
        CORE_ASSERT( dcel.m_halfedge.insert( he[2][1], he[2][1]->idx ), "New halfedge[2][1] not inserted" );
        CORE_ASSERT( dcel.m_halfedge.insert( he[2][2], he[2][2]->idx ), "New halfedge[2][2] not inserted" );
        CORE_ASSERT( dcel.m_halfedge.insert( he[3][1], he[3][1]->idx ), "New halfedge[3][1] not inserted" );
        CORE_ASSERT( dcel.m_halfedge.insert( he[3][2], he[3][2]->idx ), "New halfedge[3][2] not inserted" );
        CORE_ASSERT( dcel.m_fulledge.insert( fe[1], fe[1]->idx ), "New fulledge[1] not inserted" );
        CORE_ASSERT( dcel.m_fulledge.insert( fe[2], fe[2]->idx ), "New fulledge[2] not inserted" );
        CORE_ASSERT( dcel.m_fulledge.insert( fe[3], fe[3]->idx ), "New fulledge[3] not inserted" );
        CORE_ASSERT( dcel.m_face.insert( f[2], f[2]->idx ), "New face[2] not inserted" );
        CORE_ASSERT( dcel.m_face.insert( f[3], f[3]->idx ), "New face[3] not inserted" );
    }
}



void fulledgeCollapse( Dcel& dcel, const Index fulledge_id ) {

}



} // namespace Core
} // namespace Ra
