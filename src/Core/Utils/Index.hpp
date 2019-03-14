#ifndef RADIUMENGINE_INDEX_HPP
#define RADIUMENGINE_INDEX_HPP

#include <limits>

namespace Ra {
namespace Core {
namespace Utils {

/**
 * The Index class represent indices of a container.
 */
class Index {
  public:
    constexpr Index( int i = s_invalid );

    constexpr Index( const Index& i );

    constexpr Index& operator=( const Index& id );

    // Destructor must not be defined, we need it trivial to be constexpr
    // ~Index() { }

    /// \name Validity
    /// \{

    /**
     * Return true if the Index is valid.
     */
    constexpr bool isValid() const;

    /**
     * Return true if the Index if invalid.
     */
    // FIXME: What about the validity of Index(-2) ?
    constexpr bool isInvalid() const;

    /**
     * Make the index invalid.
     */
    constexpr void setInvalid();

    /**
     * Return an invalid index.
     */
    static constexpr Index Invalid();
    /// \}

    /// \name Values
    /// \{

    /**
     * Return the index value.
     */
    constexpr int getValue() const;

    /**
     * Set the index value.
     */
    constexpr void setValue( const int i );

    /**
     * Return the maximal index.
     */
    static constexpr Index Max();

    /**
     * Cast the index as an int to allow operator[] access on vectors.
     */
    constexpr operator int() const;
    /// \}

    /// \name Arithmetics
    /// \{

    /**
     * Increment the index.
     */
    constexpr Index& operator++();

    /**
     * Decrement the index, only if it is valid.
     */
    constexpr Index& operator--();

    /**
     * Return the increment of the index by the given offset index.
     * \note If either *this or \p id is invalid, return an invalid index.
     */
    constexpr Index operator+( const Index& id );

    /**
     * Return the increment of the index by the given offset.
     * \note No check is done for validity.
     */
    template <typename Integer>
    constexpr Index operator+( const Integer& i );

    /**
     * Return the decrement of the index by the given offset index.
     * \note If either *this or \p id is invalid, return an invalid index.
     */
    constexpr Index operator-( const Index& id );

    /**
     * Return the decrement of the index by the given offset.
     * \note No check is done for validity.
     */
    template <typename Integer>
    constexpr Index operator-( const Integer& i );

    /**
     * Return true if both *this and \p id have the same index value.
     */
    constexpr bool operator==( const Index& id );

    /**
     * Return true if the index value of *this equals \p i.
     */
    template <typename Integer>
    constexpr bool operator==( const Integer& i );

    /**
     * Return true if *this and \p id have different index value.
     */
    constexpr bool operator!=( const Index& id );

    /**
     * Return true if the index value of *this is not equal to \p i.
     */
    template <typename Integer>
    constexpr bool operator!=( const Integer& i );

    /**
     * Return true if the index value of *this is lower than \p id's.
     * \note If either *this or \p id is invalid, return false.
     */
    constexpr bool operator<( const Index& id1 );

    /**
     * Return true if the index value of *this is lower than \p i.
     */
    template <typename Integer>
    constexpr bool operator<( const Integer& i );

    /**
     * Return true if the index value of *this is lower or equal to \p id's.
     * \note If either *this or \p id is invalid, return false.
     */
    constexpr bool operator<=( const Index& id1 );

    /**
     * Return true if the index value of *this is lower or equal to \p i.
     */
    template <typename Integer>
    constexpr bool operator<=( const Integer& i );

    /**
     * Return true if the index value of *this is higher than \p id's.
     * \note If either *this or \p id is invalid, return false.
     */
    constexpr bool operator>( const Index& id1 );

    /**
     * Return true if the index value of *this is higher than \p i.
     */
    template <typename Integer>
    constexpr bool operator>( const Integer& i );

    /**
     * Return true if the index value of *this is higher or equal to \p id's.
     * \note If either *this or \p id is invalid, return false.
     */
    constexpr bool operator>=( const Index& id1 );

    /**
     * Return true if the index value of *this is higher or equal to \p i.
     */
    template <typename Integer>
    constexpr bool operator>=( const Integer& i );
    /// \}

  protected:
    /// The actual index value.
    int m_idx{s_invalid};

  private:
    /// Index value of an invalid index.
    static const int s_invalid{-1};

    /// Maximal index value.
    static const int s_maxIdx{std::numeric_limits<int>::max()};
};
} // namespace Utils
} // namespace Core
} // namespace Ra

#include <Core/Utils/Index.inl>

#endif // RADIUMENGINE_INDEX_HPP
