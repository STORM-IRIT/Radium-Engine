#pragma once

#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {
namespace Utils {

class RA_CORE_API CircularIndex
{
  public:
    /// CONSTRUCTOR
    CircularIndex( const size_t size = 1, const size_t i = 0 );
    CircularIndex( const CircularIndex& index );

    /// DESTRUCTOR
    ~CircularIndex();

    /// DIMENSION
    inline size_t size() const;
    inline void setSize( const size_t size );

    /// INDEX
    inline size_t value() const;
    inline void setValue( const size_t i );

    /// OPERATOR
    inline operator size_t();
    inline CircularIndex& operator=( const CircularIndex& id );
    inline CircularIndex& operator=( const size_t i );
    inline CircularIndex& operator++();
    inline CircularIndex& operator--();
    inline CircularIndex operator+( const int off );
    inline CircularIndex operator-( const int off );
    inline bool operator==( const CircularIndex& id ) const;
    inline bool operator<( const CircularIndex& id ) const;
    inline bool operator==( const size_t i ) const;
    inline bool operator<( const size_t i ) const;

  protected:
    /// VARIABLE
    size_t idx;
    size_t N;
};

} // namespace Utils
} // namespace Core
} // namespace Ra

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
