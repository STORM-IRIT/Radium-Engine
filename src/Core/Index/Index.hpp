#ifndef RADIUMENGINE_INDEX_HPP
#define RADIUMENGINE_INDEX_HPP

#include <limits>

namespace Ra {
namespace Core {

/// The Index class represent indices of a container.
// FIXME: What about the validity of Index(-2) ?
class Index {
  public:
    constexpr Index( int i = s_invalid );

    constexpr Index( const Index& i );

    constexpr Index& operator=( const Index& id );

    // DESTRUCTOR: Must not be defined, we need it trivial to be constexpr

    /// Return true if the Index is valid.
    constexpr bool isValid() const;

    /// Return true if the Index if invalid.
    constexpr bool isInvalid() const;

    /// Make the index invalid.
    constexpr void setInvalid();

    /// Return an invalid index.
    static constexpr Index Invalid();

    /// Return the maximal index.
    static constexpr Index Max();

    /// Return the index value.
    constexpr int getValue() const;

    /// Set the index value.
    constexpr void setValue( const int i );

    /// Cast the index to an int.
    constexpr operator int() const; // required for `array[idx]`

    /// Increment the index.
    constexpr Index& operator++();

    /// Decrement the index, only if it is valid.
    constexpr Index& operator--();

    /// Return the increment of the index by the given offset index.
    /// \note If either *this or \p id is invalid, return an invalid index.
    constexpr Index operator+( const Index& id );

    /// Return the increment of the index by the given offset.
    /// \note No check is done for validity.
    template <typename Integer>
    constexpr Index operator+( const Integer& i );

    /// Return the decrement of the index by the given offset index.
    /// \note If either *this or \p id is invalid, return an invalid index.
    constexpr Index operator-( const Index& id );

    /// Return the decrement of the index by the given offset.
    /// \note No check is done for validity.
    template <typename Integer>
    constexpr Index operator-( const Integer& i );

    /// Return true if both *this and \p id have the same index value.
    constexpr bool operator==( const Index& id );

    /// Return true if the index value of *this equals \p i.
    template <typename Integer>
    constexpr bool operator==( const Integer& i );

    /// Return true if *this and \p id have different index value.
    constexpr bool operator!=( const Index& id );

    /// Return true if the index value of *this is not equal to \p i.
    template <typename Integer>
    constexpr bool operator!=( const Integer& i );

    /// Return true if the index value of *this is lower than \p id's.
    /// \note If either *this or \p id is invalid, return false.
    constexpr bool operator<( const Index& id );

    /// Return true if the index value of *this is lower than \p i.
    template <typename Integer>
    constexpr bool operator<( const Integer& i );

    /// Return true if the index value of *this is lower or equal to \p id's.
    /// \note If either *this or \p id is invalid, return false.
    constexpr bool operator<=( const Index& id );
    template <typename Integer>

    /// Return true if the index value of *this is lower or equal to \p i.
    constexpr bool operator<=( const Integer& i );

    /// Return true if the index value of *this is higher than \p id's.
    /// \note If either *this or \p id is invalid, return false.
    constexpr bool operator>( const Index& id );
    template <typename Integer>

    /// Return true if the index value of *this is higher than \p i.
    constexpr bool operator>( const Integer& i );

    /// Return true if the index value of *this is higher or equal to \p id's.
    /// \note If either *this or \p id is invalid, return false.
    constexpr bool operator>=( const Index& id );
    template <typename Integer>

    /// Return true if the index value of *this is higher or equal to \p i.
    constexpr bool operator>=( const Integer& i );

  protected:
    /// The actual index value.
    int m_idx;

  private:
    /// Index value of an invalid index.
    static const int s_invalid = -1;

    /// Maximal index value.
    static const int s_maxIdx = std::numeric_limits<int>::max();
};
} // namespace Core
} // namespace Ra

#include <Core/Index/Index.inl>

#endif // RADIUMENGINE_INDEX_HPP
