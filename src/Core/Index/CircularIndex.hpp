#ifndef CIRCULAR_INDEX_H
#define CIRCULAR_INDEX_H

#include <Core/CoreMacros.hpp>

namespace Ra {
namespace Core {

/// The CircularIndex class represent indices of a circular container.
/// Indices are bound to [0,N-1], where N is the size of the container.
/// Incrementing a CircularIndex which is equal to N-1 will make it equal to 0.
class CircularIndex {
  public:
    CircularIndex( const uint size = 1, const int i = 0 );

    CircularIndex( const CircularIndex& index );

    inline CircularIndex& operator=( const CircularIndex& id );

    inline CircularIndex& operator=( const int i );

    ~CircularIndex();

    /// Return the size of the container.
    inline uint size() const;

    /// Set thesize of the container.
    inline void setSize( const uint size );

    /// Return the index of the element.
    inline uint value() const;

    /// Set the index of the element.
    inline void setValue( const int i );

    /// Cast the index to an int.
    inline operator int();

    /// Increment operator, modulo N.
    inline CircularIndex& operator++();

    /// Decrement operator, modulo N.
    inline CircularIndex& operator--();

    /// Increment *this by the given offset, modulo N.
    inline CircularIndex operator+( const int off );

    /// Decrement *this by the given offset, modulo N.
    inline CircularIndex operator-( const int off );

    /// Return true if both *this and \p id have the same index value.
    /// \note no check is done against the container size.
    inline bool operator==( const CircularIndex& id ) const;

    /// Return true if the value of *this is lower than \p id's.
    inline bool operator<( const CircularIndex& id ) const;

    /// Return true if the value of *this equals \p i.
    inline bool operator==( const int i ) const;

    /// Return true if the value of *this is lower than \p i.
    inline bool operator<( const int i ) const;

  protected:
    /// The actual index.
    uint idx;

    /// The container size.
    uint N;
};

} // namespace Core
} // namespace Ra

#include <Core/Index/CircularIndex.inl>

#endif // CIRCULAR_INDEX_H
