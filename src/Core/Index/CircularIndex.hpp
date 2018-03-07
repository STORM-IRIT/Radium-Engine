#ifndef CIRCULAR_INDEX_H
#define CIRCULAR_INDEX_H

#include <Core/CoreMacros.hpp>

namespace Ra {
namespace Core {

class CircularIndex {
  public:
    /// CONSTRUCTOR
    CircularIndex( const uint size = 1, const int i = 0 );
    CircularIndex( const CircularIndex& index );

    /// DESTRUCTOR
    ~CircularIndex();

    /// DIMENSION
    inline uint size() const;
    inline void setSize( const uint size );

    /// INDEX
    inline uint value() const;
    inline void setValue( const int i );

    /// OPERATOR
    inline operator int();
    inline CircularIndex& operator=( const CircularIndex& id );
    inline CircularIndex& operator=( const int i );
    inline CircularIndex& operator++();
    inline CircularIndex& operator--();
    inline CircularIndex operator+( const int off );
    inline CircularIndex operator-( const int off );
    inline bool operator==( const CircularIndex& id ) const;
    inline bool operator<( const CircularIndex& id ) const;
    inline bool operator==( const int i ) const;
    inline bool operator<( const int i ) const;

  protected:
    /// VARIABLE
    uint idx;
    uint N;
};

} // namespace Core
} // namespace Ra

#include <Core/Index/CircularIndex.inl>

#endif // CIRCULAR_INDEX_H
