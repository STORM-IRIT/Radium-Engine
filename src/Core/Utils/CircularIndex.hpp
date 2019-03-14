#ifndef CIRCULAR_INDEX_H
#define CIRCULAR_INDEX_H

#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {
namespace Utils {

/**
 * The CircularIndex class represent indices of a circular container.
 * Indices are bound to [0,N-1], where N is the size of the container.
 * Incrementing a CircularIndex which is equal to N-1 will make it equal to 0.
 */
class RA_CORE_API CircularIndex {
  public:
    CircularIndex( const size_t size = 1, const size_t i = 0 );

    CircularIndex( const CircularIndex& index );

    inline CircularIndex& operator=( const CircularIndex& id );

    inline CircularIndex& operator=( const size_t i );

    ~CircularIndex();
    /// Return the size of the container.
    inline size_t size() const;
    /// Return the size of the container.
    inline void setSize( const size_t size );
    /// Return the size of the container.
    inline size_t value() const;
    /// Return the size of the container.
    inline void setValue( const size_t i );

    /// \name Operations
    /// \{

    /**
     * Return the actual index.
     */
    inline operator size_t();

    /**
     * Increment operator, modulo N.
     */
    inline CircularIndex& operator++();

    /**
     * Decrement operator, modulo N.
     */
    inline CircularIndex& operator--();

    /**
     * Increment *this by the given offset, modulo N.
     */
    inline CircularIndex operator+( const int off );

    /**
     * Decrement *this by the given offset, modulo N.
     */
    inline CircularIndex operator-( const int off );

    /**
     * Return true if both *this and \p id have the same index value.
     * \note No check is done against the container size.
     */
    inline bool operator==( const CircularIndex& id ) const;

    /**
     * Return true if the value of *this is lower than \p id's.
     */
    inline bool operator<( const CircularIndex& id ) const;

    /**
     * Return true if the value of *this equals \p i.
     */
    inline bool operator==( const size_t i ) const;

    /**
     * Return true if the value of *this is lower than \p i.
     */
    inline bool operator<( const size_t i ) const;
    /// \}

  protected:
    size_t idx; ///< The actual index.
    size_t N;   ///< The container size.
};

} // namespace Utils
} // namespace Core
} // namespace Ra

#include <Core/Utils/CircularIndex.inl>

#endif // CIRCULAR_INDEX_H
