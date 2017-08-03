#include <Core/Mesh/DCEL/Iterator/Vertex/VVIterator.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>

namespace Ra {
namespace Core {

/// CONSTRUCTOR
VVIterator::VVIterator( Vertex_ptr& v ) : VIterator< Vertex >( v ) { }



/// DESTRUCTOR
VVIterator::~VVIterator() { }



/// LIST
/*
inline VertexList VVIterator::list() const {
    VertexList L;
    HalfEdge_ptr it = m_v->HE();
    do {
        L.push_back( it->Next()->V() );
        it = it->Prev()->Twin();
    } while( it != m_v->HE() );
    return L;
}
*/

/*
inline VertexList VVIterator::list() const
{
    VertexList L;
    HalfEdge_ptr h1 = m_v->HE();
    HalfEdge_ptr h2 = h1 != NULL ? h1->Prev()->Twin() : NULL;

    Vertex_ptr v1, v2;

    if (h1 != NULL)
    {
        CORE_ASSERT(h1->V()->idx == m_v->idx, "Halfedge does not have the right vertex reference");
        v1 = h1->Next()->V();
        L.push_back(v1);
        h1 = h1->Twin()->Next();
    }
    if (h2 != NULL)
    {
        CORE_ASSERT(h2->V()->idx == m_v->idx, "Halfedge does not have the right vertex reference");
        v2 = h2->Next()->V();
        L.push_back(v2);
        h2 = h2->Prev()->Twin();
    }

    while(h1 != NULL || h2 != NULL)
    {
        if (h1 != NULL)
        {
            CORE_ASSERT(h1->V()->idx == m_v->idx, "Halfedge does not have the right vertex reference");
            v1 = h1->Next()->V();
            if (v1 == v2) break;
            L.push_back(v1);
            h1 = h1->Twin()->Next();
        }
        if (h2 != NULL)
        {
            CORE_ASSERT(h2->V()->idx == m_v->idx, "Halfedge does not have the right vertex reference");
            v2 = h2->Next()->V();
            if (v2 == v1) break;
            L.push_back(v2);
            h2 = h2->Prev()->Twin();
        }
    }
    return L;
}
*/

inline VertexList VVIterator::list() const
{
    VertexList L;
    HalfEdge_ptr h1 = m_v->HE();
    HalfEdge_ptr h2 = h1 != NULL ? h1->Prev()->Twin() : NULL;
    Vertex_ptr v1 = h1->Next()->V();
    Vertex_ptr v2 = h2->Next()->V();
    do {
        if (h1 != NULL)
        {
            L.push_back(v1);
            if (v1 == v2) break;
            h1 = h1->Twin()->Next();
            v1 = h1->Next()->V();
        }
        if (h2 != NULL)
        {
            L.push_back(v2);
            if (v1 == v2) break;
            h2 = h2->Prev()->Twin();
            v2 = h2->Next()->V();
        }
    } while( h1 != NULL || h2 != NULL );
    return L;
}



/// OPERATOR
inline Vertex* VVIterator::operator->() const {
    return m_he->Next()->V().get();
}



} // namespace Core
} // namespace Ra
