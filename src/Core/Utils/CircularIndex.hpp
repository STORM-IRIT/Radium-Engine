#ifndef CIRCULAR_INDEX_H
#define CIRCULAR_INDEX_H

#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {
namespace Utils {

class RA_CORE_API CircularIndex {
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

#include <Core/Utils/CircularIndex.inl>

#endif // CIRCULAR_INDEX_H
