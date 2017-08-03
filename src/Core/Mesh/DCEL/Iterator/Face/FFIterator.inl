#include <Core/Mesh/DCEL/Iterator/Face/FFIterator.hpp>

#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/Face.hpp>

namespace Ra {
namespace Core {



/// CONSTRUCTOR
FFIterator::FFIterator( Face_ptr& f ) : FIterator< Face >( f ) { }



/// DESTRUCTOR
FFIterator::~FFIterator() { }



/// LIST
/*
inline FaceList FFIterator::list() const {
    FaceList L;
    HalfEdge_ptr it = m_f->HE();
    do {
        L.push_back( it->F() );
        it = it->Next();
    } while( it != m_f->HE() );
    return L;
}
*/

inline FaceList FFIterator::list() const {
    FaceList L;
    HalfEdge_ptr it = m_f->HE();

    // Neigbors faces
    do {
        if (it->Twin() != NULL)
            L.push_back(it->Twin()->F());
        it = it->Next();
    } while(it != m_f->HE());

    // Faces around vi
    it = m_f->HE();
    for (uint i = 0; i < 3; i++)
    {
        Vertex_ptr vi = it->V();
        HalfEdge_ptr h1 = vi->HE();
        HalfEdge_ptr h2 = h1 != NULL ? h1->Twin() : NULL;
        Face_ptr f1 = h1->F();
        Face_ptr f2 = h2->F();
        do {
            if (h1 != NULL)
            {
                if (f1 != m_f && f1 != L[0] &&
                    f1 != L[1] && f1 != L[2])
                {
                    L.push_back(f1);
                }
                if (f1 == f2) break;
                h1 = h1->Prev()->Twin();
                f1 = h1->F();
            }
            if (h2 != NULL)
            {
                if (f2 != m_f && f2 != L[0] &&
                    f2 != L[1] && f2 != L[2])
                {
                    L.push_back(f2);
                }
                if (f1 == f2) break;
                h2 = h2->Next()->Twin();
                f2 = h2->F();
            }
        } while( h1 != NULL || h2 != NULL );
        it = it->Next();
    }

    return L;
}

// Neighboring size
inline FaceList FFIterator::list2() const
{
    FaceList L;
    HalfEdge_ptr it = m_f->HE();

    // Vertex neighbors of the face


    //

    return L;



}



/// OPERATOR
inline Face* FFIterator::operator->() const {
    return m_he->Twin()->F().get();
}



} // namespace Core
} // namespace Ra
