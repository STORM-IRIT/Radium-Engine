#include <Core/Geometry/DCEL/Dcel.hpp>

#include <Core/Geometry/DCEL/Vertex.hpp>
#include <Core/Geometry/DCEL/HalfEdge.hpp>
#include <Core/Geometry/DCEL/Face.hpp>

namespace Ra {
namespace Core {
namespace Dcel {

/// CONSTRUCTOR
Dcel::Dcel() : IndexedObject(),
               m_vertex(),
               m_halfedge(),
               m_face() { }

Dcel::Dcel( const Index& index ) : IndexedObject( index ),
                                   m_vertex(),
                                   m_halfedge(),
                                   m_face() { }

Dcel::Dcel( const Dcel& dcel ) : IndexedObject( dcel.idx ) {
    // Clear the data
    clear();

    // Upload the vertex data, but the halfedge pointer
    for( uint i = 0; i < dcel.m_vertex.size(); ++i ) {
        Vertex_ptr dcel_v = dcel.m_vertex.at( i );
        Vertex_ptr v = new Vertex( dcel_v->idx, dcel_v->P(), dcel_v->N() );
        m_vertex.insert( v );
    }
    // Upload the halfedge data, but the halfedge pointers and the face pointer
    for( uint i = 0; i < dcel.m_halfedge.size(); ++i ) {
        HalfEdge_ptr dcel_he = dcel.m_halfedge.at( i );
        HalfEdge_ptr he = new HalfEdge( dcel_he->idx, m_vertex[dcel_he->V()->idx] );
        m_halfedge.insert( he );
    }
    // Upload the face data
    for( uint i = 0; i < dcel.m_face.size(); ++i ) {
        Face_ptr dcel_f = dcel.m_face.at( i );
        Face_ptr f = new Face( dcel_f->idx, m_halfedge[dcel_f->HE()->idx] );
        m_face.insert( f );
    }
    // Upload the remaining data, belonging to halfedges and vertices
    for( uint i = 0; i < m_halfedge.size(); ++i ) {
        auto dcel_he = dcel.m_halfedge.at( i );
        HalfEdge_ptr he  = m_halfedge[i];
        he->Next()       = m_halfedge[dcel_he->Next()->idx];
        he->Prev()       = m_halfedge[dcel_he->Prev()->idx];
        he->Twin()       = m_halfedge[dcel_he->Twin()->idx];
        he->F()          = m_face[dcel_he->F()->idx];
        he->V()->HE()    = m_halfedge[dcel_he->V()->HE()->idx];
    }
}

/// DESTRUCTOR
Dcel::~Dcel() {
    for( uint i = 0; i < m_vertex.size(); ++i ) {
        delete m_vertex[i];
    }
    for( uint i = 0; i < m_halfedge.size(); ++i ) {
        delete m_halfedge[i];
    }
    for( uint i = 0; i < m_face.size(); ++i ) {
        delete m_face[i];
    }
}

} // namespace DCEL
} // namespace Core
} // namespace Ra

