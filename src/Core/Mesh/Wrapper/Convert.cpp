#include <Core/Mesh/Wrapper/Convert.hpp>

#include <map>

#include <Core/Mesh/TriangleMesh.hpp>

#include <Core/Containers/MakeShared.hpp>
#include <Core/Mesh/DCEL/Dcel.hpp>
#include <Core/Mesh/DCEL/Face.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/Vertex.hpp>

namespace Ra {
namespace Core {

Twin::Twin() {
    m_id[0] = uint( -1 );
    m_id[1] = uint( -1 );
}

Twin::Twin( const uint i, const uint j ) {
    m_id[0] = std::min( i, j );
    m_id[1] = std::max( i, j );
}

bool Twin::operator==( const Twin& twin ) const {
    return ( ( m_id[0] == twin.m_id[0] ) && ( m_id[1] == twin.m_id[1] ) );
}

bool Twin::operator<( const Twin& twin ) const {
    return ( ( m_id[0] < twin.m_id[0] ) ||
             ( ( m_id[0] == twin.m_id[0] ) && ( m_id[1] < twin.m_id[1] ) ) );
}

void convert( const TriangleMesh& mesh, Dcel& dcel ) {
    dcel.clear();
    // Create vertices
    for ( unsigned int i = 0; i < mesh.vertices().size(); ++i )
    {
        Vector3 p = mesh.vertices().at( i );
        Vector3 n = mesh.normals().at( i );
        Vertex_ptr v = std::shared_ptr<Vertex>( new Vertex( p, n ) );
        CORE_ASSERT( ( v != nullptr ), "Vertex_ptr == nullptr" );
        v->idx = dcel.m_vertex.insert( v );

        CORE_ASSERT( v->idx.isValid(), "Vertex not inserted" );
    }
    /// TWIN DATA
    std::map<Twin, Index> he_table;
    // Create faces and halfedges
    for ( const auto& t : mesh.m_triangles )
    {
        // Create the halfedges
        HalfEdgeList he;
        for ( uint i = 0; i < 3; ++i )
        {
            he.push_back( std::shared_ptr<HalfEdge>( new HalfEdge() ) );
            CORE_ASSERT( ( he[i] != nullptr ), "HalfEdge_ptr == nullptr" );
        }
        // Create the face
        Face_ptr f = Ra::Core::make_shared<Face>( he[0] );
        CORE_ASSERT( ( f != nullptr ), "Face_ptr == nullptr" );
        f->idx = dcel.m_face.insert( f );
        CORE_ASSERT( f->idx.isValid(), "Face not inserted" );

        // Create the connections
        for ( uint i = 0; i < 3; ++i )
        {

            CORE_ASSERT( dcel.m_vertex.contains( t[i] ), "vertex not found" );

            Vertex_ptr& v = dcel.m_vertex[t[i]];
            v->setHE( he[i] );
            he[i]->setV( v );
            he[i]->setNext( he[( i + 1 ) % 3] );
            he[i]->setPrev( he[( i + 2 ) % 3] );
            he[i]->setF( f );
            he[i]->idx = dcel.m_halfedge.insert( he[i] );
            CORE_ASSERT( he[i]->idx.isValid(), "HalfEdge not inserted" );
            /// TWIN SEARCH
            Twin twin( t[i], t[( i + 1 ) % 3] );
            // Search the right twin
            auto it = he_table.find( twin );
            if ( it == he_table.end() )
            {
                // If not present, add it
                he_table[twin] = he[i]->idx;
            } else
            {
                // If found, set it and erase it
                CORE_ASSERT( dcel.m_halfedge.contains( it->second ), "Map error" );
                CORE_ASSERT( dcel.m_halfedge[it->second]->idx == it->second, "Map error" );
                he[i]->setTwin( dcel.m_halfedge[it->second] );
                dcel.m_halfedge[it->second]->setTwin( he[i] );
                // Create the fulledge
                FullEdge_ptr fe = std::shared_ptr<FullEdge>( new FullEdge( he[i] ) );
                CORE_ASSERT( ( fe != nullptr ), "FullEdge_ptr == nullptr" );

                fe->idx = dcel.m_fulledge.insert( fe );
                CORE_ASSERT( fe->idx.isValid(), "FullEdge not inserted" );

                he[i]->setFE( fe );
                he[i]->Twin()->setFE( fe );
                he_table.erase( it );
            }
        }
    }
}

void convert( const Dcel& dcel, TriangleMesh& mesh ) {
    const uint v_size = dcel.m_vertex.size();
    const uint f_size = dcel.m_face.size();
    mesh.vertices().resize( v_size );
    mesh.normals().resize( v_size );
    mesh.m_triangles.resize( f_size );
    std::map<Index, uint> v_table;
    for ( uint i = 0; i < v_size; ++i )
    {
        const Vertex_ptr& v = dcel.m_vertex.at( i );
        const Vector3 p = v->P();
        const Vector3 n = v->N();
        mesh.vertices()[i] = p;
        mesh.normals()[i] = n;
        v_table[v->idx] = i;
    }
    for ( uint i = 0; i < f_size; ++i )
    {
        const Face_ptr& f = dcel.m_face.at( i );
        Triangle T;
        T[0] = v_table[f->HE()->V()->idx];
        T[1] = v_table[f->HE()->Next()->V()->idx];
        T[2] = v_table[f->HE()->Prev()->V()->idx];
        mesh.m_triangles[i] = T;
    }
}

} // namespace Core
} // namespace Ra
