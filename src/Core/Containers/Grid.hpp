#ifndef RADIUMENGINE_GRID_HPP
#define RADIUMENGINE_GRID_HPP
#include <Core/RaCore.hpp>
#include <Eigen/Core>

namespace Ra
{
    namespace Core
    {
        /// This class stores a D-dimensional grid of elements of arbitrary type.
        /// in a contiguous memory block. Elements are stored in column-major order.
        /// e.g. for a 3x3x3 array the element vector looks like
        ///  [A000, A100, A200, A010,... A222].
        /// Elements are accessible with a D-dimensional Vector, or linearly with
        /// iterators, thanks to the std-like interface provided.
        template <typename T, uint D>
        class Grid
        {

        public:
            // public types and constants.
            static const uint Dimension = D; /// Dimension of our grid
            typedef Eigen::Matrix<uint, D, 1> IdxVector; /// A vector of the size of the grid along each dimension.
            typedef Eigen::Matrix< int, D, 1> OffsetVector; /// A vector of signed offsets.

            /// This class implements an iterator though elements of the grid that
            /// can be referenced with a linear index or a D-dimensional uint vector.
            class Iterator
            {
            public:

                //
                // Construction
                //

                /// Constructor from size and linear index.
                Iterator( const IdxVector& size, uint startIdx = 0 );

                /// Constructor from size and vector index.
                Iterator( const IdxVector& size, const IdxVector& startIdx );

                /// Constructor from grid and linear index.
                Iterator( const Grid<T,D>& grid, uint startIdx = 0 );

                /// Constructor fron grid and vector index.
                Iterator( const Grid<T,D>& grid, const IdxVector& startIdx );

                /// Default copy constructor and assignment operator.
                Iterator ( const Iterator& other ) = default;
                Iterator& operator= ( const Iterator& other  ) = default;

                /// Cast to the an iterator in a different type grid.
                template <typename T2 >
                typename Grid<T2,D>::Iterator cast() const;

                //
                // Basic getters and setters
                //

                /// Set an existing iterator to a linear index value.
                inline void setFromLinear( uint idx );

                /// Set an existing iterator to a vector index value.
                inline void setFromVector( const IdxVector& idx );

                /// Get the current linear index.
                inline uint getLinear() const;

                /// Get the current vector index.
                inline IdxVector getVector() const;

                /// Get the associated grid size.
                inline const IdxVector& getGridSize() const;

                /// Returns true if the current index is out of the grid.
                inline bool isOut() const;

                /// Returns true if the current index is inside the grid.
                inline bool isIn() const;

                //
                // Increment and decrement operators
                //

                /// Advance the iterator to the next-element
                inline Iterator& operator++ ();
                inline Iterator  operator++ ( int );

                /// Move the iterator back to the previous element
                inline Iterator& operator-- ();
                inline Iterator  operator-- ( int );

                //
                // Add and substract operators with linear offset
                //

                /// Advance the iterator with an offset of i elements
                inline Iterator& operator+= ( uint i );

                /// Rewind the iterator with an offset of i elements
                inline Iterator& operator-= ( uint i );

                //
                // Add and substract operators with vector offset
                //

                // Note : these convenience functions do not do any kind
                // of check. If you get out of bounds on one dimension, the iterator
                // might still be valid but not pointing to the element you think
                // it should.

                inline Iterator& operator+= ( const IdxVector& idx );
                inline Iterator& operator-= ( const IdxVector& idx );
                inline Iterator& operator+= ( const OffsetVector& idx );

                /// Returns true if adding offset idx with current iterator will act
                /// as expected and false if it will put it out of bounds.
                bool isValidOffset( const OffsetVector& idx );

                //
                // Comparison operators
                //

                // Note : comparing iterators of different grid sizes will assert.

                inline bool operator== ( const Iterator& other ) const;
                inline bool operator<  ( const Iterator& other ) const;

                //
                // Extra operators
                //

                // Declare binary operators directly (see C++ faq 35.16)
                friend Iterator operator+(const Iterator &it, uint i) {  Iterator copy(it); return copy += i; }
                friend Iterator operator-(const Iterator &it, uint i) {  Iterator copy(it); return copy -= i; }
                friend Iterator operator+(const Iterator &it, const IdxVector &idx) { Iterator copy (it); return copy += idx; }
                friend Iterator operator-(const Iterator &it, const IdxVector &idx) { Iterator copy (it); return copy -= idx; }
                friend Iterator operator+(const Iterator &it, const OffsetVector &idx) { Iterator copy (it); return copy += idx; }

                // Extraneous comparison operators default implementation.
                inline bool operator!=(const Iterator& rhs) const {return !(*this == rhs) ;}
                inline bool operator> (const Iterator& rhs) const {return    rhs  <  *this;}
                inline bool operator<=(const Iterator& rhs) const {return !(*this >  rhs) ;}
                inline bool operator>=(const Iterator& rhs) const {return !(*this <  rhs) ;}

            private:
                const IdxVector& m_sizes; /// Size of the underlying grid
                uint m_index; /// Current linear index of the iterator

            };

        public:
            RA_CORE_ALIGNED_NEW

            //
            // Constructors
            //

            /// Construct a grid of a given size and fill it with the given value.
            Grid( const IdxVector& size = IdxVector::Zero(), const T& val = T() );
            /// Construct a grid of a given size with values in ()-major format
            Grid( const IdxVector& size, const T* values );

            /// Copy constructor and assignment operator.
            Grid( const Grid<T, D>& other ) = default;
            Grid& operator= ( const Grid<T, D>& other ) = default;

            //
            // Basic getters
            //

            /// Returns the number of elements stored.
            inline uint size() const;
            /// Returns the size vector (a D-dimensional vector with the size along each dimension).
            inline const IdxVector& sizeVector() const;
            /// Returns true if the grid is empty (i.e. if size() ==0).
            inline bool empty() const;
            /// Erases all data and makes the grid empty.
            inline void clear();

            //
            // Element access
            //

            /// Access an element with a D-dimensionnal index.
            // Note that since initializer lists are implicitely convertible to
            // Eigen vectors, you can call grid.at({x,y,z})
            inline const T& at( const IdxVector& idx ) const;
            inline T& at( const IdxVector& idx );

            /// Access an element with a linear index.
            inline const T& at( uint idx ) const;
            inline T& at( uint idx );

            /// Access an element with an iterator.
            inline const T& at( const Iterator& it) const;
            inline T& at( const Iterator& it);

            /// Read only access to the underlying data.
            inline const T* data() const;

            /// Read-write access to the underlying data.
            inline T* data();

            //
            // std::iterators-like interface
            //

            /// Get an iterator on this grid at the first element.
            inline Iterator begin();
            inline Iterator begin() const;

            /// Get an interator on this grid past the last element.
            inline Iterator end();
            inline Iterator end() const;

        protected:
            /// Indicate the extends of the grid along each dimension.
            IdxVector m_size;
            /// Storage for the grid data.
            std::vector<T> m_data;

        };
    }
}

#include <Core/Containers/Grid.inl>

#endif //RADIUMENGINE_GRID_HPP
