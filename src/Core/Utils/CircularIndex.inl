#include <Core/Utils/CircularIndex.hpp>

namespace Ra {
namespace Core {
namespace Utils {

/// DIMENSION
inline size_t CircularIndex::size() const {
    return N;
}

inline void CircularIndex::setSize( const size_t size ) {
    N = size;
}

/// INDEX
inline size_t CircularIndex::value() const {
    return idx;
}

inline void CircularIndex::setValue( const size_t i ) {
    const size_t n = N;
    idx            = ( n + ( i % n ) ) % n;
}

/// OPERATOR
inline CircularIndex::operator size_t() {
    return idx;
}

inline CircularIndex& CircularIndex::operator=( const CircularIndex& id ) {
    N   = id.N;
    idx = id.idx;
    return *this;
}

inline CircularIndex& CircularIndex::operator=( const size_t i ) {
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

inline CircularIndex CircularIndex::operator+( const int off ) {
    return CircularIndex( N, idx + size_t( int( N ) + off % int( N ) ) );
}

inline CircularIndex CircularIndex::operator-( const int off ) {
    return ( *this + ( -off ) );
}

inline bool CircularIndex::operator==( const CircularIndex& id ) const {
    return ( idx == id.idx );
}

inline bool CircularIndex::operator<( const CircularIndex& id ) const {
    return ( idx < id.idx );
}

inline bool CircularIndex::operator==( const size_t i ) const {
    return ( idx == CircularIndex( N, i ) );
}

inline bool CircularIndex::operator<( const size_t i ) const {
    return ( idx < size_t( CircularIndex( N, i ) ) );
}

} // namespace Utils
} // namespace Core
} // namespace Ra
