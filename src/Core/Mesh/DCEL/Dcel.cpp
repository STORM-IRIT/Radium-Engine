#include <Core/Mesh/DCEL/Dcel.hpp>

#include <map>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/Face.hpp>

namespace Ra {
namespace Core {



/// CONSTRUCTOR
Dcel::Dcel( const Index& index ) :
    IndexedObject( index ),
    m_vertex(),
    m_halfedge(),
    m_face() { }



Dcel::Dcel( const Dcel& dcel ) :
    IndexedObject( dcel.idx ),
    m_vertex(),
    m_halfedge(),
    m_face() {

    // Mapping between indices of the two DCELs
    std::map< Index, Index > v_table;
    std::map< Index, Index > he_table;
    std::map< Index, Index > f_table;

    // Upload the vertex data, but the halfedge pointer
    for( uint i = 0; i < dcel.m_vertex.size(); ++i ) {
        Vertex_ptr dcel_v = dcel.m_vertex.at( i );
        Vertex_ptr v      = std::make_shared< Vertex >( dcel_v->P(), dcel_v->N() );
        m_vertex.insert( v, v->idx );
        v_table[dcel_v->idx] = v->idx;
    }

    // Upload the halfedge data, but the halfedge pointers and the face pointer
    for( uint i = 0; i < dcel.m_halfedge.size(); ++i ) {
        HalfEdge_ptr dcel_he = dcel.m_halfedge.at( i );
        HalfEdge_ptr he      = std::make_shared< HalfEdge >( m_vertex[v_table[dcel_he->V()->idx]] );
        m_halfedge.insert( he, he->idx );
        he_table[dcel_he->idx] = he->idx;
    }
    // Upload the face data
    for( uint i = 0; i < dcel.m_face.size(); ++i ) {
        Face_ptr dcel_f = dcel.m_face.at( i );
        Face_ptr f      = std::make_shared< Face >( m_halfedge[he_table[dcel_f->HE()->idx]] );
        m_face.insert( f, f->idx );
        f_table[dcel_f->idx] = f->idx;
    }

    // Upload the remaining data, belonging to halfedges and vertices
    for( uint i = 0; i < m_halfedge.size(); ++i ) {
        HalfEdge_ptr dcel_he = dcel.m_halfedge.at( i );
        HalfEdge_ptr he      = m_halfedge[i];
        he->setNext( m_halfedge[he_table[dcel_he->Next()->idx]] );
        he->setPrev( m_halfedge[he_table[dcel_he->Prev()->idx]] );
        he->setTwin( m_halfedge[he_table[dcel_he->Twin()->idx]] );
        he->setF( m_face[f_table[dcel_he->F()->idx]] );
        he->V()->setHE( m_halfedge[he_table[dcel_he->V()->HE()->idx]] );
    }
}



/// DESTRUCTOR
Dcel::~Dcel() { }



} // namespace Core
} // namespace Ra

