#include <Core/Geometry/FullEdgeOperation.hpp>

#include <Core/Container/Index.hpp>

#include <Core/Geometry/DCEL/Dcel.hpp>
#include <Core/Geometry/DCEL/Definition.hpp>
#include <Core/Geometry/DCEL/FullEdge.hpp>
#include <Core/Geometry/DCEL/HalfEdge.hpp>
#include <Core/Geometry/DCEL/Vertex.hpp>

namespace Ra {
namespace Core {
namespace Geometry {



template <typename T>
void insertInMap( Container::IndexMap<T>& map, T& t ) {
    t->idx = map.insert( t );
    CORE_ASSERT( t->idx.isValid(), "Not inserted" );
}

void fulledgeSplit( Core::Dcel& dcel, const Container::Index fulledge_id ) {
    if ( dcel.m_fulledge.contains( fulledge_id ) )
    {
        FullEdge_ptr ptr = dcel.m_fulledge.at( fulledge_id );
        // Declare the data
        Vertex_ptr v;
        HalfEdge_ptr he[4][3];
        FullEdge_ptr fe[4];
        Face_ptr f[4];

        // Create the new vertex
        v = std::shared_ptr<Vertex>(
            new Vertex( ( 0.5 * ( ptr->V( 0 )->P() + ptr->V( 1 )->P() ) ),
                        ( ( ptr->V( 0 )->N() + ptr->V( 1 )->N() ).normalized() ), ptr->HE( 1 ) ) );

        // Prepare the halfedges
        he[0][0] = ptr->HE( 0 )->Prev();
        he[0][1] = ptr->HE( 0 );
        he[0][2] = std::shared_ptr<HalfEdge>( new HalfEdge( v ) );
        he[1][0] = ptr->HE( 1 )->Next();
        he[1][1] = std::shared_ptr<HalfEdge>( new HalfEdge( ptr->HE( 1 )->Prev()->V() ) );
        he[1][2] = ptr->HE( 1 );
        he[2][0] = ptr->HE( 1 )->Prev();
        he[2][1] = std::shared_ptr<HalfEdge>( new HalfEdge( ptr->HE( 1 )->V() ) );
        he[2][2] = std::shared_ptr<HalfEdge>( new HalfEdge( v ) );
        he[3][0] = ptr->HE( 0 )->Next();
        he[3][1] = std::shared_ptr<HalfEdge>( new HalfEdge( ptr->HE( 0 )->Prev()->V() ) );
        he[3][2] = std::shared_ptr<HalfEdge>( new HalfEdge( v ) );

        // Prepare the fulledges
        fe[0] = ptr;
        fe[1] = std::shared_ptr<FullEdge>( new FullEdge( he[1][1] ) );
        fe[2] = std::shared_ptr<FullEdge>( new FullEdge( he[2][1] ) );
        fe[3] = std::shared_ptr<FullEdge>( new FullEdge( he[3][1] ) );

        // Prepare the faces
        f[0] = he[0][0]->F();
        f[1] = he[1][0]->F();
        f[2] = std::shared_ptr<Face>( new Face( he[2][0] ) );
        f[3] = std::shared_ptr<Face>( new Face( he[3][0] ) );

        // Set the data
        he[1][2]->setV( v );
        for ( uint i = 0; i < 4; ++i )
        {
            f[i]->setHE( he[i][0] );
            for ( uint j = 0; j < 3; ++j )
            {
                he[i][j]->setNext( he[i][( j + 1 ) % 3] );
                he[i][j]->setPrev( he[i][( j + 2 ) % 3] );
                he[i][j]->setFE( fe[i] );
                he[i][j]->setF( f[i] );
            }
            he[i][1]->setTwin( he[( i + 1 ) % 4][2] );
            he[i][2]->setTwin( he[( i + 3 ) % 4][1] );
        }

        // Insert new data
        insertInMap( dcel.m_vertex, v );
        insertInMap( dcel.m_halfedge, he[0][2] );
        insertInMap( dcel.m_halfedge, he[1][1] );
        insertInMap( dcel.m_halfedge, he[2][1] );
        insertInMap( dcel.m_halfedge, he[2][2] );
        insertInMap( dcel.m_halfedge, he[3][1] );
        insertInMap( dcel.m_halfedge, he[3][2] );
        insertInMap( dcel.m_fulledge, fe[1] );
        insertInMap( dcel.m_fulledge, fe[2] );
        insertInMap( dcel.m_fulledge, fe[3] );
        insertInMap( dcel.m_face, f[2] );
        insertInMap( dcel.m_face, f[3] );
    }
}

void fulledgeCollapse( Dcel& dcel, const Container::Index fulledge_id ) {}


} // namespace Geometry
} // namespace Core
} // namespace Ra
