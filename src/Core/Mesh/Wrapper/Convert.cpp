#include <Core/Mesh/Wrapper/Convert.hpp>

#include <map>

#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Mesh/DCEL/Dcel.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/Face.hpp>

namespace Ra {
namespace Core {

Twin::Twin() {
    m_id[0] = uint(-1);
    m_id[1] = uint(-1);
}

Twin::Twin( const uint i, const uint j ) {
    m_id[0] = std::min( i, j );
    m_id[1] = std::max( i, j );
}

bool Twin::operator==( const Twin& twin ) const {
    return ( ( m_id[0] == twin.m_id[0] ) && ( m_id[1] == twin.m_id[1] ) );
}

bool Twin::operator< ( const Twin& twin ) const {
    return ( ( m_id[0] < twin.m_id[0] ) || ( ( m_id[0] == twin.m_id[0] ) && ( m_id[1] < twin.m_id[1] ) ) );
}



void convert( const TriangleMesh& mesh, Dcel& dcel ) {
    dcel.clear();
    // Create vertices
    for( unsigned int i = 0; i < mesh.m_vertices.size(); ++i ) {
        Vector3 p = mesh.m_vertices.at( i );
        Vector3 n = mesh.m_normals.at( i );
        Vertex_ptr v = std::make_shared< Vertex >( p, n );
        dcel.m_vertex.insert( v, v->idx );
    }
    /// TWIN DATA
    std::map< Twin, Index > he_table;
    // Create faces and halfedges
    for( const auto& t : mesh.m_triangles ) {
        // Create the halfedges
        HalfEdge_ptr he[3];
        for( uint i = 0; i < 3; ++i ) {
            he[i] = std::make_shared< HalfEdge >();
            dcel.m_halfedge.insert( he[i], he[i]->idx );
        }
        // Create the face
        Face_ptr f = std::make_shared< Face >( he[0] );
        dcel.m_face.insert( f, f->idx );
        // Create the connections
        for( uint i = 0; i < 3; ++i ) {
            Vertex_ptr& v = dcel.m_vertex[ t[i] ];
            v->setHE( he[i] );
            he[i]->setV( v );
            he[i]->setNext( he[( i + 1 ) % 3] );
            he[i]->setPrev( he[( i + 2 ) % 3] );
            he[i]->setF( f );
            dcel.m_halfedge.insert( he[i], he[i]->idx );
            /// TWIN SEARCH
            Twin twin( t[i], t[( i + 1 ) % 3]);
            // Search the right twin
            auto it = he_table.find( twin );
            if( it == he_table.end() ) {
                // If not present, add it
                he_table[twin] = he[i]->idx;
            } else {
                // If found, set it and erase it
                he[i]->setTwin( dcel.m_halfedge[it->second] );
                dcel.m_halfedge[it->second]->setTwin( he[i] );
                he_table.erase( it );
            }
        }
    }
}



void convert( const Dcel& dcel, TriangleMesh& mesh ) {
    const uint v_size = dcel.m_vertex.size();
    const uint f_size = dcel.m_face.size();
    mesh.m_vertices.resize( v_size );
    mesh.m_normals.resize( v_size );
    mesh.m_triangles.resize( f_size );
    std::map< Index, uint > v_table;
    for( uint i = 0; i < v_size; ++i ) {
        const Vertex_ptr& v = dcel.m_vertex.at( i );
        const Vector3 p = v->P();
        const Vector3 n = v->N();
        mesh.m_vertices[i] = p;
        mesh.m_normals[i]  = n;
        v_table[ v->idx ] = i;
    }
    for( uint i = 0; i < f_size; ++i ) {
        const Face_ptr& f = dcel.m_face.at( i );
        Triangle T;
        T[0] = v_table[ f->HE()->V()->idx ];
        T[1] = v_table[ f->HE()->Next()->V()->idx ];
        T[2] = v_table[ f->HE()->Prev()->V()->idx ];
        mesh.m_triangles[i] = T;
    }
}




} // namespace Core
} // namespace Ra
