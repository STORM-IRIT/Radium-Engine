#include "Grid.hpp"

namespace Ra
{
    namespace Core
    {
        // Anonymous helper functions to convert to/from multi-dimensional indices
        // If we want to  make the grid row-major we just need to overload these functions
        // by reversing the loops.
        namespace
        {

            template<typename T, uint D>
            typename Grid<T, D>::IdxVector linearToIdxVector( uint linIdx, const typename Grid<T, D>::IdxVector& size )
            {
                typename Grid<T, D>::IdxVector result = Grid<T, D>::IdxVector::Zero();

                for ( uint i = 0; i < D ; ++i )
                {
                    result[i] = linIdx % size[i];
                    linIdx = linIdx / size[i];
                }
                return result;
            }

            template<typename T, uint D>
            uint idxVectorToLinear( const typename Grid<T, D>::IdxVector& vecIdx, const typename Grid<T, D>::IdxVector& size )
            {
                uint result = 0;
                uint dimProd = 1;
                for ( uint i = 0; i < D ; ++i )
                {
                    result += vecIdx[i] * dimProd;
                    dimProd *= size[i];
                }
                return result;
            }
        }

        //
        // Constructors
        //

        template<typename T, uint D>
        Grid<T, D>::Grid( const Grid::IdxVector& size, const T& val )
            : m_size( size ), m_data( size.prod(), val ) { }

        template<typename T, uint D>
        Grid<T, D>::Grid( const Grid::IdxVector& size, const T* values )
            : m_size( size ), m_data( size.prod(), values ) { }

        //
        // Vector size and data management.
        //


        template<typename T, uint D>
        inline uint Grid<T, D>::size() const
        {
            CORE_ASSERT( m_data.size() == m_size.prod(), "Inconsistent grid size" );
            return m_data.size();
        }

        template<typename T, uint D>
        inline const typename Grid<T, D>::IdxVector& Grid<T, D>::sizeVector() const
        {
            CORE_ASSERT( m_data.size() == m_size.prod(), "Inconsistent grid size" );
            return m_size;
        }

        template<typename T, uint D>
        inline bool Grid<T, D>::empty() const
        {
            CORE_ASSERT( m_data.size() == m_size.prod(), "Inconsistent grid size" );
            return m_data.empty();
        }

        template<typename T, uint D>
        inline void Grid<T, D>::clear()
        {
            m_data.clear();
            m_size = IdxVector::Zero();
            CORE_ASSERT( empty(), "Inconsistent grid" );
        }

        template<typename T, uint D>
        inline const T* Grid<T, D>::data() const
        {
            return m_data.data();
        }

        template<typename T, uint D>
        inline T* Grid<T, D>::data()
        {
            return m_data.data();
        }

        //
        // Individual element access.
        //

        template<typename T, uint D>
        inline const T& Grid<T, D>::at( const Grid<T, D>::IdxVector& idx ) const
        {
            const uint linIdx = idxVectorToLinear<T, D> ( idx, sizeVector() );
            CORE_ASSERT( linIdx < m_data.size(), "Invalid vector index" );
            return m_data[linIdx];
        }

        template<typename T, uint D>
        inline T& Grid<T, D>::at( const Grid<T, D>::IdxVector& idx )
        {
            const uint linIdx = idxVectorToLinear<T, D> ( idx, sizeVector() );
            CORE_ASSERT( linIdx < m_data.size(), "Invalid vector index" );
            return m_data[linIdx];
        }

        template<typename T, uint D>
        inline const T& Grid<T, D>::at( uint idx ) const
        {
            CORE_ASSERT( idx < m_data.size(), "Invalid vector index" );
            return m_data[idx];
        }

        template<typename T, uint D>
        inline T& Grid<T, D>::at( uint idx )
        {
            CORE_ASSERT( idx < m_data.size(), "Invalid vector index" );
            return m_data[idx];
        }

        template<typename T, uint D>
        inline typename Grid<T, D>::IdxVector
        Grid<T, D>::getIdxVector( const Grid<T, D>::ConstIterator& iterator ) const
        {
            const uint diff = ( iterator - cbegin() );
            return linearToIdxVector<T, D> ( diff, sizeVector() );
        }

        template<typename T, uint D>
        inline typename Grid<T,D>::Iterator
        Grid<T,D>::getIterator(const Grid::IdxVector &idx)
        {
           return this->begin() + idxVectorToLinear<T,D>( idx, m_size );
        }

        template<typename T, uint D>
        inline typename Grid<T,D>::ConstIterator
        Grid<T,D>::getConstIterator(const Grid::IdxVector &idx) const
        {
           return this->cbegin() + idxVectorToLinear<T,D>( idx, m_size );
        }

        //
        // Iterators stuff.
        //

        template<typename T, uint D>
        inline typename Grid<T, D>::Iterator Grid<T, D>::begin()
        {
            return m_data.begin();
        }

        template<typename T, uint D>
        inline typename Grid<T, D>::ConstIterator Grid<T, D>::begin() const
        {
            return m_data.begin();
        }

        template<typename T, uint D>
        inline typename Grid<T, D>::ConstIterator Grid<T, D>::cbegin() const
        {
            return m_data.cbegin();
        }


        template<typename T, uint D>
        inline typename Grid<T, D>::Iterator Grid<T, D>::end()
        {
            return m_data.end();
        }

        template<typename T, uint D>
        inline typename Grid<T, D>::ConstIterator Grid<T, D>::end() const
        {
            return m_data.end();
        }

        template<typename T, uint D>
        inline typename Grid<T, D>::ConstIterator Grid<T, D>::cend() const
        {
            return m_data.cend();
        }
    }
}
