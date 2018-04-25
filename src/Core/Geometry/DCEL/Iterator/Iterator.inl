#include <Core/Geometry/DCEL/Iterator/Iterator.hpp>

#include <Core/Geometry/DCEL/HalfEdge.hpp>

namespace Ra {
namespace Core {

/// CONSTRUCTOR
template <typename OBJECT>
Iterator<OBJECT>::Iterator() : m_he( nullptr ) {}

/// DESTRUCTOR
template <typename OBJECT>
Iterator<OBJECT>::~Iterator() {}

/// OPERATOR
template <typename OBJECT>
inline Iterator<OBJECT>& Iterator<OBJECT>::operator+=( const uint n ) const {
    for ( uint i = 0; i < n; ++i )
    {
        ++( *this );
    }
    return *this;
}

template <typename OBJECT>
inline Iterator<OBJECT>& Iterator<OBJECT>::operator-=( const uint n ) const {
    for ( uint i = 0; i < n; ++i )
    {
        --( *this );
    }
    return *this;
}

} // namespace Core
} // namespace Ra
