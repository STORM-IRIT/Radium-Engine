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
        inline const T& Grid<T, D>::at ( uint idx ) const
        {
            CORE_ASSERT ( idx < m_data.size(), "Invalid vector index" );
            return m_data[idx];
        }

        template<typename T, uint D>
        inline T& Grid<T, D>::at ( uint idx )
        {
            CORE_ASSERT ( idx < m_data.size(), "Invalid vector index" );
            return m_data[idx];
        }

        template<typename T, uint D>
        inline typename Grid<T, D>::IdxVector
        Grid<T, D>::getIdxVector( const Grid<T, D>::ConstIterator& iterator ) const
        {
            const uint diff = ( iterator - cbegin() );
            return linearToIdxVector<T, D> ( diff, sizeVector() );
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
        
        namespace GridUtils
        {
            /// Compute 2d gradients with central finite difference.
            /// Borders of the grid are untouched.
            /// @param scale : if 'vals' represent a 2D function what is the range of (x, y)
            /// variables? Scale defines that x ranges to [0; scale.x] and y ranges to
            /// [0; scale.y].
            /// @tparam Real  : a real number type
            /// @tparam Grad2 : gradient must define '.x' and '.y' attributes
            template<typename Real, typename Grad2>
            void grad_2d(Grid2_ref<Real> vals,
                         Grid2_ref<Grad2> grads,
                         const Vector2 scale = Vector2(1., 1.))
            {
                assert( vals.size() == grads.size());

                Real step_x = scale(0) / (vals.size()(0) - 1);
                Real step_y = scale(1) / (vals.size()(1) - 1);

                Idx2 off( vals.size(), 1, 1);
                for(Idx2 sub_idx( vals.size() - Vector2i(2, 2), 0); sub_idx.is_in(); ++sub_idx)
                {
                    Idx2 idx = off + sub_idx.to_2d();

                    Real x_plus_h  = vals( idx + Vector2i( 1, 0) );
                    Real x_minus_h = vals( idx + Vector2i(-1, 0) );
                    Real grad_x = (x_plus_h - x_minus_h) / ((Real)2 * step_x);

                    Real y_plus_h  = vals( idx + Vector2i(0,  1) );
                    Real y_minus_h = vals( idx + Vector2i(0, -1) );
                    Real grad_y = (y_plus_h - y_minus_h) / ((Real)2 * step_y);

                    grads( idx )(0) = grad_x;
                    grads( idx )(1) = grad_y;
                }
            }
        }
    }
}
