#include <Core/Mesh/DCEL/Iterator/Vertex/VFIterator.hpp>

#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/Face.hpp>

namespace Ra {
namespace Core {



/// CONSTRUCTOR
inline VFIterator::VFIterator( Vertex_ptr& v ) : VIterator< Face >( v ) { }



/// DESTRUCTOR
inline VFIterator::~VFIterator() { }



/// LIST
/*
inline FaceList VFIterator::list() const {
    FaceList L;
    HalfEdge_ptr it = m_v->HE();
    do {
        L.push_back( it->F() );
        it = it->Prev()->Twin();
    } while( it != m_v->HE() );
    return L;
}
*/

/*
inline FaceList VFIterator::list() const
{
    FaceList L;
    HalfEdge_ptr h1 = m_v->HE();
    HalfEdge_ptr h2 = h1 != NULL ? h1->Twin() : NULL;

    Face_ptr f1, f2;

    if (h1 != NULL)
    {
        f1 = h1->F();
        L.push_back(f1);
        h1 = h1->Prev()->Twin();
    }
    if (h2 != NULL)
    {
        f2 = h2->F();
        L.push_back(f2);
        h2 = h2->Next()->Twin();
    }

    while(h1 != NULL || h2 != NULL)
    {
        if (h1 != NULL)
        {
            f1 = h1->F();
            if (f1 == f2) break;
            L.push_back(f1);
            h1 = h1->Prev()->Twin();
        }
        if (h2 != NULL)
        {
            f2 = h2->F();
            if (f2 == f1) break;
            L.push_back(f2);
            h2 = h2->Next()->Twin();
        }
    }
    return L;
}
*/

inline FaceList VFIterator::list() const
{
    FaceList L;
    HalfEdge_ptr h1 = m_v->HE();
    HalfEdge_ptr h2 = h1 != NULL ? h1->Twin() : NULL;
    Face_ptr f1 = h1->F();
    Face_ptr f2 = h2->F();
    do {
        if (h1 != NULL)
        {
            L.push_back(f1);
            if (f1 == f2) break;
            h1 = h1->Prev()->Twin();
            f1 = h1->F();
        }
        if (h2 != NULL)
        {
            L.push_back(f2);
            if (f1 == f2) break;
            h2 = h2->Next()->Twin();
            f2 = h2->F();
        }
    } while( h1 != NULL || h2 != NULL );
    return L;
}

/// N-RING
void VFIterator::nRing(uint n, std::set<Face_ptr, VFIterator::compareFacePtr>& adjFacesSet)
{
    if (n == 0) return;
    FaceList adjFaces = list();
    for (uint t = 0; t < adjFaces.size(); t++)
    {
        adjFacesSet.insert(adjFaces[t]);
        HalfEdge_ptr he = adjFaces[t]->HE();
        for (uint i = 0; i < 3; i++)
        {
            if (he->V()->idx == m_v->idx)
            {
                VFIterator newvfIt = VFIterator(he->Next()->V());
                newvfIt.nRing(n-1, adjFacesSet);
                break;
            }
            he = he->Next();
        }
    }
}




/// OPERATOR
inline Face* VFIterator::operator->() const {
    return m_he->F().get();
}



} // namespace Core
} // namespace Ra
