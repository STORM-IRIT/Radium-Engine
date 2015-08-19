#ifndef RADIUMENGINE_IDX2_HPP
#define RADIUMENGINE_IDX2_HPP

#include <Core/Math/LinearAlgebra.hpp>

namespace Ra
{
    namespace Core
    {
        /**
         * @struct Idx2
         * @brief 2d grid index with conversion to 2d and linear storage
         *
         * Use case to look up every elements of a grid of 16x16
         * @code
         *
         * Vector2i s(16, 16);
         * for(Idx2 idx(s, 0); idx.is_in(); ++idx)
         * {
         *      Vector2i idx_2d = idx.to_2d(); // or .to_Vector2i() will do the same
         *      int   i      = idx.to_linear();
         *
         *      // i == idx_2d.x + s.x * idx_2d.y
         * }
         * @endcode
         *
         * Looking up a sub grid 10^2 inside a grid 128^2:
         * @code
         * Vector2i sub_grid_s(10, 10);
         * Vector2i grid_s(128, 128);
         * Idx2 offset(grid_s, offx, offy);
         * for(Idx2 idx(sub_grid_s, 0); idx.is_in(); ++idx)
         * {
         *     int i = (offset + idx.to_Vector2i()).to_linear();
         *     // 'i' is in linear coordinates of the grid 128^2 but will only look up
         *     // a sub block of size 10^2 starting from the offset in the 128^2 grid
         * }
         * @endcode
         */
        struct Idx2
        {


            // -------------------------------------------------------------------------
            /// @name Constructors
            // -------------------------------------------------------------------------

            inline
            Idx2() : _size( -1, -1 ), _id( -1 ) { }

            /// Build index from a linear index
            inline
            Idx2( const Vector2i& size, int idx ) : _size( size ), _id( idx ) { }

            /// Build index from a 3d index
            inline
            Idx2( const Vector2i& size, int ix, int iy ) : _size( size )
            {
                _id = to_linear( _size, ix, iy );
            }

            /// Build index from a 3d index
            inline
            Idx2( const Vector2i& size, const Vector2i& pos ) : _size( size )
            {
                set_2d( pos );
            }

            // -------------------------------------------------------------------------
            /// @name Set index position
            // -------------------------------------------------------------------------

            inline
            void set_linear( int i )
            {
                _id = i;
            }

            inline
            void set_2d( const Vector2i& p )
            {
                set_2d( p( 0 ), p( 1 ) );
            }

            inline
            void set_2d( int x, int y )
            {
                _id = to_linear( _size, x, y );
            }

            inline
            int to_linear() const
            {
                return _id;
            }

            // -------------------------------------------------------------------------
            /// @name Get index position
            // -------------------------------------------------------------------------

            inline
            Vector2i to_2d() const
            {
                Vector2i r;
                to_2d( r( 0 ), r( 1 ) );
                return r;
            }

            inline
            Vector2i to_Vector2i() const
            {
                return to_2d();
            }

            inline
            void to_2d( int& x, int& y ) const
            {
                x = _id % _size( 0 );
                y = _id / _size( 0 );
            }

            // -------------------------------------------------------------------------
            /// @name Other methods
            // -------------------------------------------------------------------------

#ifdef __CUDACC__
            int3 to_int3() const
            {
                return make_int3( _size( 0 ), _size( 0 ), _id );
            }
#endif

            inline
            int size_linear() const
            {
                return _size( 0 ) * _size( 1 ) * _size( 2 );
            }

            inline
            Vector2i size() const
            {
                return _size;
            }

            /// A valid index is positive as well as its size
            inline
            bool is_valid() const
            {
                return _id >= 0 && size_linear() >= 0;
            }

            /// Does the index is out of its bounds (defined at construction)
            inline bool is_out() const
            {
                return !is_in();
            }

            /// Does the index is inside its bounds (defined at construction)
            inline
            bool is_in() const
            {
                return ( _id < size_linear() ) && ( _id >= 0 );
            }

            // -------------------------------------------------------------------------
            /// @name Operators overload
            // -------------------------------------------------------------------------

            inline
            Idx2 operator++( )
            {
                return Idx2( _size, ++_id );
            }

            inline Idx2 operator++( int )
            {
                return Idx2( _size, _id++ );
            }

            inline
            Idx2 operator--( )
            {
                return Idx2( _size, --_id );
            }

            inline
            Idx2 operator--( int )
            {
                return Idx2( _size, _id-- );
            }

            inline
            bool operator==( const Idx2& i ) const
            {
                return _size == i._size && _id == i._id;
            }

            inline
            bool operator!=( const Idx2& i ) const
            {
                return _size != i._size || _id != i._id;
            }

            inline
            Idx2 operator =( const Idx2& i )
            {
                _size = i._size;
                _id = i._id;
                return *this;
            }

            inline friend
            Idx2 operator+ ( const Idx2& id, const Vector2i& v )
            {
                Vector2i this_idx = id.to_2d();
                return Idx2( id._size, this_idx + v );
            }

            inline friend
            Idx2 operator+ ( const Vector2i& v, const Idx2& id )
            {
                return id + v;
            }

        private:

            static inline
            int to_linear( const Vector2i& size, int x, int y )
            {
                return x + size( 0 ) * y;
            }

            Vector2i _size; ///< 3d size the index is looking up
            int      _id;   ///< Linear index

            // WARNING: these operators should not be used/implemented since:
            // (they don't really make sense) || (are to ambigus to decypher when used)
#if 0
            bool operator<=( const Idx2& ) const
            {
                return false;
            }
            bool operator>=( const Idx2& ) const
            {
                return false;
            }
            bool operator< ( const Idx2& ) const
            {
                return false;
            }
            bool operator> ( const Idx2& ) const
            {
                return false;
            }

            Idx2 operator- ( const Idx2& ) const
            {
                return Idx2();
            }
            Idx2 operator+ ( const Idx2& ) const
            {
                return Idx2();
            }
            Idx2 operator+=( const Idx2& )
            {
                return Idx2();
            }
            Idx2 operator-=( const Idx2& )
            {
                return Idx2();
            }

            bool operator==( int ) const
            {
                return false;
            }
            bool operator!=( int ) const
            {
                return false;
            }
            bool operator<=( int ) const
            {
                return false;
            }
            bool operator>=( int ) const
            {
                return false;
            }
            bool operator> ( int ) const
            {
                return false;
            }
            bool operator< ( int ) const
            {
                return false;
            }

            Idx2 operator+ ( int )  const
            {
                return Idx2();
            }
            Idx2 operator- ( int )  const
            {
                return Idx2();
            }
            Idx2 operator+=( int )
            {
                return Idx2();
            }
            Idx2 operator-=( int )
            {
                return Idx2();
            }
#endif
        };
    }
}

#endif // TOOL_BOX_IDX2_HPP
