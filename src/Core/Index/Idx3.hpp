#ifndef RADIUMENGINE_IDX3_HPP
#define RADIUMENGINE_IDX3_HPP
#include <Core/RaCore.hpp>
#include <Core/Math/LinearAlgebra.hpp>
namespace Ra
{
    namespace Core
    {

        /**
         * @struct Idx3
         * @brief 3d grid index with conversion to 3d and linear storage
         *
         * Use case to look up every elements of a grid of 16x16x16
         * @code
         *
         * Vector3i s(16, 16, 16);
         * for(Idx3 idx(s, 0); idx.is_in(); ++idx)
         * {
         *      Vector3i idx_3d = idx.to_3d(); // or .to_Vector3i() will do the same
         *      int   i      = idx.to_linear();
         *
         *      // i == idx_3d.x + s.x * idx_3d.y + (s.x * s.y) * idx_3d.z
         * }
         * @endcode
         *
         * Looking up a sub grid 10^3 inside a grid 128^3:
         * @code
         * Vector3i sub_grid_s(10, 10, 10);
         * Vector3i grid_s(128, 128, 128);
         * Idx3 offset(grid_s, offx, offy, offz);
         * for(Idx3 idx(sub_grid_s, 0); idx.is_in(); ++idx)
         * {
         *     int i = (offset + idx.to_Vector3i()).to_linear();
         *     // 'i' is in linear coordinates of the grid 128^3 but will only look up
         *     // a sub block of size 10^3 starting from the offset in the 128^3 grid
         * }
         * @endcode
         */
        struct Idx3
        {

            constexpr static uint InvalidId = uint(-1);

            // -------------------------------------------------------------------------
            /// @name Constructors
            // -------------------------------------------------------------------------

            inline
            Idx3() :  _size( ), _id( InvalidId ) { }

            /// Build index from a linear index
            inline
            Idx3( const Vector3ui& size, uint idx ) : _size( size ), _id( idx ) { }

            /// Build index from a 3d index
            inline
            Idx3( const Vector3ui& size, uint ix, uint iy, uint iz ) : _size( size )
            {
                _id = to_linear( _size, ix, iy, iz );
            }

            /// Build index from a 3d index
            inline
            Idx3( const Vector3ui& size, const Vector3ui& pos ) : _size( size )
            {
                /* Should I authorize invalid indices?
                assert(pos.x >= 0 && pos.x < size.x);
                assert(pos.y >= 0 && pos.y < size.y);
                assert(pos.z >= 0 && pos.z < size.z);
                */
                set_3d( pos );
            }

            // -------------------------------------------------------------------------
            /// @name Set index position
            // -------------------------------------------------------------------------

            inline
            void set_linear( uint i )
            {
                _id = i;
            }

            inline
            void set_3d( uint x, uint y, uint z )
            {
                _id = to_linear( _size, x, y, z );
            }

            inline
            void set_3d( const Vector3ui& p )
            {
                set_3d( p( 0 ), p( 1 ), p( 2 ) );
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
            Vector3ui to_3d() const
            {
                Vector3ui r;
                to_3d( r( 0 ), r( 1 ), r( 2 ) );
                return r;
            }
            inline
            void to_3d( uint& x, uint& y, uint& z ) const
            {
                x = _id % _size( 0 );
                z = _id / _size( 0 );
                y =  z  % _size( 1 );
                z =  z  / _size( 1 );
            }

            // -------------------------------------------------------------------------
            /// @name Other methods
            // -------------------------------------------------------------------------


            inline
            int size_linear() const
            {
                return _size( 0 ) * _size( 1 ) * _size( 2 );
            }

            inline
            Vector3ui size() const
            {
                return _size;
            }

            /// A valid index is positive as well as its size
            // TODO: I'm not sure we should implement this method
            // deleting it would allow to check with assert() index bounds at construction
            inline
            bool is_valid() const
            {
                return _id != InvalidId && size_linear() >= 0;
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
                return ( _id != InvalidId &&  _id < size_linear() ) ;
            }

            // -------------------------------------------------------------------------
            /// @name Operators overload
            // -------------------------------------------------------------------------

            inline
            Idx3 operator++( )
            {
                return Idx3( _size, ++_id );
            }

            inline Idx3 operator++( int )
            {
                return Idx3( _size, _id++ );
            }

            inline
            Idx3 operator--( )
            {
                return Idx3( _size, --_id );
            }

            inline
            Idx3 operator--( int )
            {
                return Idx3( _size, _id-- );
            }

            inline
            bool operator==( const Idx3& i ) const
            {
                return _size == i._size && _id == i._id;
            }

            inline
            bool operator!=( const Idx3& i ) const
            {
                return _size != i._size || _id != i._id;
            }

            inline
            Idx3 operator =( const Idx3& i )
            {
                _size = i._size;
                _id = i._id;
                return *this;
            }

            inline friend
            Idx3 operator+ ( const Idx3& id, const Vector3ui& v )
            {
                Vector3ui this_idx = id.to_3d();
                return Idx3( id._size, this_idx + v );
            }

            inline friend
            Idx3 operator+ ( const Vector3ui& v, const Idx3& id )
            {
                return id + v;
            }

        private:

            static inline
            uint to_linear( const Vector3ui& size, uint x, uint y, uint z )
            {
                return x + size( 0 ) * ( y + size( 1 ) * z );
            }

            Vector3ui _size; ///< 3d size of the grid the index is looking up
            uint   _id;   ///< Linear index

        };
    }
}

#endif // TOOL_BOX_IDX3_HPP
