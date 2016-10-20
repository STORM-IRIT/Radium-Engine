#include <Core/Mesh/DCEL/Iterator/Edge/EdgeIterator.hpp>

#include <Core/Mesh/DCEL/HalfEdge.hpp>

namespace Ra {
namespace Core {

/// CONSTRUCTOR
template < typename OBJECT >
EIterator< OBJECT >::EIterator( HalfEdge_ptr& he ) :
    Iterator< OBJECT >(){
    this->m_he = he;
}






/// DESTRUCTOR
template < typename OBJECT >
EIterator< OBJECT >::~EIterator() { }






/// SIZE
template < typename OBJECT >
inline uint EIterator< OBJECT >::size() const {
    uint i = 0;
    HalfEdge_ptr it = this->m_he;
    do {
        it = it->Prev()->Twin();
        ++i;
    } while( it != this->m_he );
    it = this->m_he;
    do {
        it = it->Prev()->Twin();
        ++i;
    } while( it != this->m_he->Twin() );
    return i;
}


/// RESET
template < typename OBJECT >
inline void EIterator< OBJECT >::reset()
{
    //this->m_he = m_v->HE();
}


/// OPERATOR
template < typename OBJECT >
inline EIterator< OBJECT >& EIterator< OBJECT >::operator=( const EIterator& it )
{
    this->m_he = it.m_he;
    return *this;
}

template < typename OBJECT >
inline EIterator< OBJECT >& EIterator< OBJECT >::operator++()
{
    this->m_he = this->m_he->Prev()->Twin();
    return *this;
}

template < typename OBJECT >
inline EIterator< OBJECT >& EIterator< OBJECT >::operator--()
{
    this->m_he = this->m_he->Twin()->Next();
    return *this;
}


template < typename OBJECT >
inline bool EIterator< OBJECT >::operator==( const EIterator& it ) const
{
    return ( this->m_he == it.m_he );
}



} // namespace Core
} // namespace Ra
