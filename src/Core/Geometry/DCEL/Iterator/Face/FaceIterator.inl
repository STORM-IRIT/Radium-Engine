#include <Core/Geometry/DCEL/Iterator/Face/FaceIterator.hpp>

#include <Core/Geometry/DCEL/Face.hpp>
#include <Core/Geometry/DCEL/HalfEdge.hpp>

namespace Ra {
namespace Core {

/// CONSTRUCTOR
template <typename OBJECT>
FIterator<OBJECT>::FIterator( Face_ptr& f ) : Iterator<OBJECT>(), m_f( f ) {
    this->m_he = m_f->HE();
}

/// DESTRUCTOR
template <typename OBJECT>
FIterator<OBJECT>::~FIterator() {}

/// SIZE
template <typename OBJECT>
inline uint FIterator<OBJECT>::size() const {
    uint i = 0;
    HalfEdge_ptr it = m_f->HE();
    do
    {
        it = it->Next();
        ++i;
    } while ( it != m_f->HE() );
    return i;
}

/// RESET
template <typename OBJECT>
inline void FIterator<OBJECT>::reset() {
    this->m_he = m_f->HE();
}

/// OPERATOR
template <typename OBJECT>
inline FIterator<OBJECT>& FIterator<OBJECT>::operator=( const FIterator& it ) {
    m_f = it.m_fs;
    this->m_he = it.m_he;
    return *this;
}

template <typename OBJECT>
inline FIterator<OBJECT>& FIterator<OBJECT>::operator++() {
    this->m_he = this->m_he->Next();
    return *this;
}

template <typename OBJECT>
inline FIterator<OBJECT>& FIterator<OBJECT>::operator--() {
    this->m_he = this->m_he->Prev();
    return *this;
}

template <typename OBJECT>
inline bool FIterator<OBJECT>::operator==( const FIterator& it ) const {
    return ( ( m_f == it.m_f ) && ( this->m_he == it.m_he ) );
}

} // namespace Core
} // namespace Ra
