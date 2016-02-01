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

            // We use vector iterators.
            typedef typename std::vector<T>::iterator Iterator;
            typedef typename std::vector<T>::const_iterator ConstIterator;

        public:
            RA_CORE_ALIGNED_NEW

            // Construct a grid of a given size and fill it with the given value.
            Grid( const IdxVector& size = IdxVector::Zero(), const T& val = T() );
            // Construct a grid of a given size with values in ()-major format
            Grid( const IdxVector& size, const T* values );

            // Copy constructor and assignment operator.
            Grid( const Grid<T, D>& other ) = default;
            Grid& operator= ( const Grid<T, D>& other ) = default;

            /// Returns the number of elements stored.
            inline uint size() const;
            /// Returns the size vector (a D-dimensional vector with the size along each dimension).
            inline const IdxVector& sizeVector() const;
            /// Returns true if the grid is empty (i.e. if size() ==0).
            inline bool empty() const;
            /// Erases all data and makes the grid empty.
            inline void clear();

            /// Retunrs the D-dimensional index vector corresponding to the given iterator.
            inline IdxVector getIdxVector( const ConstIterator& it ) const;

            inline Iterator getIterator( const IdxVector& idx );
            inline ConstIterator getConstIterator( const IdxVector& idx) const;

            /// Access an element with a D-dimensionnal index.
            /// Note that since initializer lists are implicitely convertible to
            /// Eigen vectors, you can call grid.at({x,y,z})
            inline const T& at( const IdxVector& idx ) const;
            inline T& at( const IdxVector& idx );

            /// Access an element with a linear index.
            inline const T& at( uint idx ) const;
            inline T& at( uint idx );

            /// Read only access to the underlying data.
            inline const T* data() const;
            inline T* data();

            // std::iterators-like interface
            inline Iterator begin();
            inline ConstIterator begin() const;
            inline ConstIterator cbegin() const;

            inline Iterator end();
            inline ConstIterator end() const;
            inline ConstIterator cend() const;

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
