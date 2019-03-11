#ifndef RADIUMENGINE_GRID_HPP
#define RADIUMENGINE_GRID_HPP

#include <vector>

#include <Core/RaCore.hpp>
#include <Eigen/Core>

namespace Ra {
namespace Core {
/**
 * This class stores a D-dimensional grid of elements of arbitrary type.
 * in a contiguous memory block. Elements are stored in column-major order.
 * e.g.\ for a 3x3x3 array the element vector looks like
 *  [A000, A100, A200, A010,... A222].
 * Elements are accessible with a D-dimensional Vector, or linearly with
 * iterators, thanks to the std-like interface provided.
 */
template <typename T, uint D>
class Grid {

  public:
    /// Dimension of our grid
    static const uint Dimension = D;

    /**
     * A vector of the size of the grid along each dimension.
     */
    using IdxVector = Eigen::Matrix<uint, D, 1>;

    /**
     * A vector of signed offsets.
     */
    using OffsetVector = Eigen::Matrix<int, D, 1>;

    /**
     * This class implements an iterator though elements of the grid that
     * can be referenced with a linear index or a D-dimensional uint vector.
     */
    class Iterator {
      public:
        /**
         * Constructor from size and linear index.
         */
        Iterator( const IdxVector& size, uint startIdx = 0 );

        /**
         * Constructor from size and vector index.
         */
        Iterator( const IdxVector& size, const IdxVector& startIdx );

        /**
         * Constructor from grid and linear index.
         */
        Iterator( const Grid<T, D>& grid, uint startIdx = 0 );

        /**
         * Constructor from grid and vector index.
         */
        Iterator( const Grid<T, D>& grid, const IdxVector& startIdx );

        /**
         * Default copy constructor.
         */
        Iterator( const Iterator& other ) = default;

        /**
         * Default assignment operator.
         */
        Iterator& operator=( const Iterator& other ) = default;

        /**
         * Cast to the an iterator in a different type grid.
         */
        template <typename T2>
        typename Grid<T2, D>::Iterator cast() const;

        /// \name Getters and setters
        /// \{

        /**
         * Set an existing iterator to a linear index value.
         */
        inline void setFromLinear( uint idx );

        /**
         * Set an existing iterator to a vector index value.
         */
        inline void setFromVector( const IdxVector& idx );

        /**
         * Returns the current linear index.
         */
        inline uint getLinear() const;

        /**
         * Returns the current vector index.
         */
        inline IdxVector getVector() const;

        /**
         * Returns the associated grid size.
         */
        inline const IdxVector& getGridSize() const;
        /// \}

        /// \name Status queries
        /// \{

        /**
         * Returns true if the current index is out of the grid.
         */
        inline bool isOut() const;

        /**
         * Returns true if the current index is inside the grid.
         */
        inline bool isIn() const;

        /// Returns true if adding offset idx with current iterator will act
        /// as expected and false if it will put it out of bounds.
        bool isValidOffset( const OffsetVector& idx );
        /// \}

        /// \name Iterator comparison
        /// \{

        /**
         * Returns true if both \p this and \p other point to the same grid
         * element, false otherwise.
         * \note Comparing iterators of different grid sizes will assert.
         */
        inline bool operator==( const Iterator& other ) const;

        /**
         * Returns true if \p this and \p other do not point to the same grid
         * element, false otherwise.
         * \note Comparing iterators of different grid sizes will assert.
         */
        inline bool operator!=( const Iterator& rhs ) const { return !( *this == rhs ); }

        /**
         * Returns true if \p this points to a grid element stored before the
         * one \p other points to, false otherwise.
         * \note Comparing iterators of different grid sizes will assert.
         */
        inline bool operator<( const Iterator& other ) const;

        /**
         * Returns true if \p this points to a grid element stored after the
         * one \p other points to, false otherwise.
         * \note Comparing iterators of different grid sizes will assert.
         */
        inline bool operator>( const Iterator& rhs ) const { return rhs < *this; }

        /**
         * Returns true if \p this points to a grid element stored before the
         * one \p other points to, or if they point to the same element; false otherwise.
         * \note Comparing iterators of different grid sizes will assert.
         */
        inline bool operator<=( const Iterator& rhs ) const { return !( *this > rhs ); }

        /**
         * Returns true if \p this points to a grid element stored after the
         * one \p other points to, or if they point to the same element; false otherwise.
         * \note Comparing iterators of different grid sizes will assert.
         */
        inline bool operator>=( const Iterator& rhs ) const { return !( *this < rhs ); }
        /// \}

        /// \name Iterator operations
        /// \{

        /**
         * Advance the iterator to the next-element.
         * \warning No check is done against the dimensions bounds on the result.
         *          see isValidOffset().
         */
        inline Iterator& operator++();

        /**
         * Advance the iterator to the next-element.
         * \warning No check is done against the dimensions bounds on the result.
         *          see isValidOffset().
         */
        inline Iterator operator++( int );

        /**
         * Move the iterator back to the previous element.
         * \warning No check is done against the dimensions bounds on the result.
         *          see isValidOffset().
         */
        inline Iterator& operator--();

        /**
         * Move the iterator back to the previous element.
         * \warning No check is done against the dimensions bounds on the result.
         *          see isValidOffset().
         */
        inline Iterator operator--( int );

        /**
         * Advance the iterator with an offset of \p i elements.
         * \warning No check is done against the dimensions bounds on the result.
         *          see isValidOffset().
         */
        inline Iterator& operator+=( uint i );

        /**
         * Rewind the iterator with an offset of \p i elements.
         * \warning No check is done against the dimensions bounds on the result.
         *          see isValidOffset().
         */
        inline Iterator& operator-=( uint i );

        /**
         * Advance the iterator by the offset, in each dimension, given by \p idx.
         * \note \p idx must be a valid grid offset, see isValidOffset.
         */
        inline Iterator& operator+=( const IdxVector& idx );

        /**
         * Rewind the iterator by the offset, in each dimension, given by \p idx.
         * \note \p idx must be a valid grid offset, see isValidOffset.
         */
        inline Iterator& operator-=( const IdxVector& idx );

        /**
         * Advance the iterator by the offset, in each dimension, given by \p idx.
         * \note \p idx must be a valid grid offset, see isValidOffset.
         * \note Negative offsets perform rewind.
         */
        inline Iterator& operator+=( const OffsetVector& idx );

        /**
         * Creates an iterator from \p it advanced with an offset of \p i elements.
         */
        friend Iterator operator+( const Iterator& it, uint i ) {
            Iterator copy( it );
            return copy += i;
        }

        /**
         * Creates an iterator from \p it rewound with an offset of \p i elements.
         */
        friend Iterator operator-( const Iterator& it, uint i ) {
            Iterator copy( it );
            return copy -= i;
        }

        /**
         * Creates an iterator from \p it advanced with the offset, in each
         * dimension, given by \p idx.
         * \note \p idx must be a valid grid offset, see isValidOffset.
         */
        friend Iterator operator+( const Iterator& it, const IdxVector& idx ) {
            Iterator copy( it );
            return copy += idx;
        }

        /**
         * Creates an iterator from \p it rewound with the offset, in each
         * dimension, given by \p idx.
         * \note \p idx must be a valid grid offset, see isValidOffset.
         */
        friend Iterator operator-( const Iterator& it, const IdxVector& idx ) {
            Iterator copy( it );
            return copy -= idx;
        }

        /**
         * Creates an iterator from \p it advanced the offset, in each
         * dimension, given by \p idx.
         * \note \p idx must be a valid grid offset, see isValidOffset.
         * \note Negative offsets perform rewind.
         */
        friend Iterator operator+( const Iterator& it, const OffsetVector& idx ) {
            Iterator copy( it );
            return copy += idx;
        }
        /// \}

      private:
        const IdxVector& m_sizes; ///< Size of the underlying grid.
        uint m_index;             ///< Current linear index of the iterator.
    };

  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  public:
    /**
     * Construct a grid of a given size and fill it with the given value.
     */
    Grid( const IdxVector& size = IdxVector::Zero(), const T& val = T() );

    /**
     * Construct a grid of a given size with values in ()-major format.
     */
    Grid( const IdxVector& size, const T* values );

    /**
     * Copy constructor and assignment operator.
     */
    Grid( const Grid<T, D>& other ) = default;
    Grid& operator=( const Grid<T, D>& other ) = default;

    /**
     * Erases all data and makes the grid empty.
     */
    inline void clear();

    /// \name Status queries
    /// \{

    /**
     * Returns the number of elements stored.
     */
    inline uint size() const;

    /**
     * Returns the size vector (a D-dimensional vector with the size along each dimension).
     */
    inline const IdxVector& sizeVector() const;

    /**
     * Returns true if the grid is empty (i.e.\ if size() ==0).
     */
    inline bool empty() const;
    /// \}

    /// \name Element access
    /// \{
    // Note that since initializer lists are implicitly convertible to
    // Eigen vectors, you can call grid.at({x,y,z})

    /**
     * Access an element with a D-dimensional index.
     */
    inline const T& at( const IdxVector& idx ) const;

    /**
     * Access an element with a D-dimensional index.
     */
    inline T& at( const IdxVector& idx );

    /**
     * Access an element with a linear index.
     */
    inline const T& at( uint idx ) const;

    /**
     * Access an element with a linear index.
     */
    inline T& at( uint idx );

    /**
     * Access an element with an iterator.
     */
    inline const T& at( const Iterator& it ) const;

    /**
     * Access an element with an iterator.
     */
    inline T& at( const Iterator& it );

    /**
     * Read only access to the underlying data.
     */
    inline const T* data() const;

    /**
     * Read-write access to the underlying data.
     */
    inline T* data();
    /// \}

    /// \name Iterator interface
    /// \{

    /**
     * Returns an iterator on this grid at the first element.
     */
    inline Iterator begin();

    /**
     * Returns an iterator on this grid at the first element.
     */
    inline Iterator begin() const;

    /**
     * Returns an iterator on this grid past the last element.
     */
    inline Iterator end();

    /**
     * Returns an iterator on this grid past the last element.
     */
    inline Iterator end() const;
    /// \}

  protected:
    /// Indicate the extends of the grid along each dimension.
    IdxVector m_size;

    /// Storage for the grid data.
    std::vector<T> m_data;
};
} // namespace Core
} // namespace Ra

#include <Core/Containers/Grid.inl>

#endif // RADIUMENGINE_GRID_HPP
