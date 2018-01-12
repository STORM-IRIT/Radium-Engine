#include <Core/Index/CircularIndex.hpp>

namespace Ra {
namespace Core {

/// DIMENSION
inline uint CircularIndex::size() const {
    return N;
}

inline void CircularIndex::setSize( const uint size ) {
    N = size;
}

/// INDEX
inline uint CircularIndex::value() const {
    return idx;
}

inline void CircularIndex::setValue( const int i ) {
    const int n = N;
    idx = ( n + ( i % n ) ) % n;
}

/// OPERATOR
inline CircularIndex::operator int() {
    return idx;
}

inline CircularIndex& CircularIndex::operator= ( const CircularIndex& id ) {
    N = id.N; idx = id.idx;
    return *this;
}

inline CircularIndex& CircularIndex::operator= ( const int i ) {
    setValue( i );
    return *this;
}

inline CircularIndex& CircularIndex::operator++() {
    setValue( idx + 1 );
    return *this;
}

inline CircularIndex& CircularIndex::operator--() {
    setValue( idx - 1 );
    return *this;
}

inline CircularIndex CircularIndex::operator+ ( const int off ) {
    return CircularIndex( N, idx + off );
}

inline CircularIndex CircularIndex::operator- ( const int off ) {
    return ( *this + ( -off ) );
}

inline bool CircularIndex::operator==( const CircularIndex& id ) const {
    return ( idx == id.idx );
}

inline bool CircularIndex::operator< ( const CircularIndex& id ) const {
    return ( idx < id.idx );
}

inline bool CircularIndex::operator==( const int i ) const {
    return ( idx == CircularIndex( N, i ) );
}

inline bool CircularIndex::operator< ( const int i ) const {
    return ( idx < uint(CircularIndex( N, i )) );
}

} // namespace Core
} // namespace Ra

