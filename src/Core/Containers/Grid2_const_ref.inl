#include <Core/Containers/Grid2_const_ref.hpp>
#include <Core/Containers/Grid3.hpp>
#include <Core/Containers/Grid2.hpp>

namespace Ra
{
    namespace Core
    {
        /// @brief Abstract class to point to a 2d slice of a 3d grid
        template<typename T>
        struct Grid2_ref_slice : public Grid2<T>
        {
            Grid2_ref_slice( const Grid2_ref_slice<T>& copy ) :
                Grid2<T>( copy.size ),
                Grid2_ref<T>( *this ),
                _grid_ref( copy._grid_ref ),
                _slice( copy._slice ),
                _r0( copy._r0 ),
                _r1( copy._r1 )
            {
                //Grid2<T>::_size    = copy._size;
                //Grid2<T>::_pad_off = copy._pad_off;
            }

            Grid2_ref_slice( Grid3<T>& g, int slice, Range r0, Range r1 ) :
                Grid2<T>( Vector2i( r0.nb_elts(), r1.nb_elts() ) ),
                _grid_ref( g ),
                _slice( slice ),
                _r0( r0 ),
                _r1( r1 )
            {
            }

            virtual       T& get_val( int, int )       = 0;
            virtual const T& get_val( int, int ) const = 0;

        protected:
            // You must call this for each children ctor.
            //void init_size(const Vector2i& alloc_size) {
            //    Grid2<T>::_size    = alloc_size;
            //    //Grid2<T>::_pad_off = padd_off;
            //}

            /// 3d grid reference for the 2d grid slice
            Grid3<T>& _grid_ref;
            /// slice index in the 3d grid
            /// (x, y or z direction it depends on the child class interpretation)
            int _slice;
            /// Ranges are to be initialized by children classes
            Range _r0, _r1;
        };


        // -----------------------------------------------------------------------------

        template<typename T>
        struct Grid2_ref_xy : public Grid2_ref_slice<T>
        {

            typedef Grid2_ref_slice<T> Pt;

            Grid2_ref_xy( Grid3<T>& g, Range x, Range y, int z ) :
                Grid2_ref_slice<T>( g, z, x, y )
            {
                //this->init_size( Vector2i(x.nb_elts(), y.nb_elts()) );
            }

            T& get_val( int x, int y )
            {
                assert( x < Pt::_size.x() && y < Pt::_size.y() );
                assert( x >= 0 && y >= 0 );
                return Pt::_grid_ref( Vector3i( Pt::_r0._a + x, Pt::_r1._a + y, Pt::_slice ) );
            }

            const T& get_val( int x, int y ) const
            {
                assert( x < Pt::_size.x() && y < Pt::_size.y() );
                assert( x >= 0 && y >= 0 );
                return Pt::_grid_ref( Vector3i( Pt::_r0._a + x, Pt::_r1._a + y, Pt::_slice ) );
            }
        };

        // -----------------------------------------------------------------------------

        template<typename T>
        struct Grid2_ref_xz : public Grid2_ref_slice<T>
        {

            typedef Grid2_ref_slice<T> Pt;

            Grid2_ref_xz( Grid3<T>& g, Range x, int y, Range z ) :
                Grid2_ref_slice<T>( g, y, x, z )
            {
                //this->init_size( Vector2i(x.nb_elts(), z.nb_elts()), g.get_padd_offset().xz() );
            }

            T& get_val( int x, int y )
            {
                assert( x < Pt::_size.x && y < Pt::_size.y );
                assert( x >= 0 && y >= 0 );
                return Pt::_grid_ref( Pt::_r0._a + x, Pt::_slice, Pt::_r1._a + y );
            }

            const T& get_val( int x, int y ) const
            {
                assert( x < Pt::_size.x && y < Pt::_size.y );
                assert( x >= 0 && y >= 0 );
                return Pt::_grid_ref( Pt::_r0._a + x, Pt::_slice, Pt::_r1._a + y );
            }
        };

        // -----------------------------------------------------------------------------

        template<typename T>
        struct Grid2_ref_yz : public Grid2_ref_slice<T>
        {

            typedef Grid2_ref_slice<T> Pt;

            Grid2_ref_yz( Grid3<T>& g, int x , Range y, Range z ) :
                Grid2_ref_slice<T>( g, x, y, z )
            {
                //this->init_size( Vector2i(y.nb_elts(), z.nb_elts()), g.get_padd_offset().yz() );
            }

            T& get_val( int x, int y )
            {
                assert( x < Pt::_size.x && y < Pt::_size.y );
                assert( x >= 0 && y >= 0 );
                return Pt::_grid_ref( Pt::_slice, Pt::_r0._a + x, Pt::_r1._a + y );
            }
            const T& get_val( int x, int y ) const
            {
                assert( x < Pt::_size.x && y < Pt::_size.y );
                assert( x >= 0 && y >= 0 );
                return Pt::_grid_ref( Pt::_slice, Pt::_r0._a + x, Pt::_r1._a + y );
            }
        };

        // -----------------------------------------------------------------------------
        // Implemens of Grid2_ref_const
        // -----------------------------------------------------------------------------

        template<typename T>
        Grid2_const_ref<T>::Grid2_const_ref() : _grid_ref_const( 0 ), _counter( 0 )
        {
            _grid_ref_allocated = false;
            _counter  = new Ref_counter();
            _counter->add_ref();
        }

        // -----------------------------------------------------------------------------

        template<typename T>
        Grid2_const_ref<T>::Grid2_const_ref( const Grid2<T>& g )
        {
            _grid_ref_allocated = false;
            _grid_ref_const = &g;
            _counter        = new Ref_counter();
            _counter->add_ref();
        }

        // -----------------------------------------------------------------------------

        template<typename T>
        Grid2_const_ref<T>

        Grid2_const_ref<T>::make_xy( const Grid3<T>& g, Range x, Range y, int z )
        {
            Grid2_const_ref<T> ref;
            ref._grid_ref_allocated = true;
            // Ok it's a hack but it avoids to complicate the architecture
            // '_grid_ref_const' is constant so 'g' should not be modified through it
            // only Grid2_ref_xy constructor might do that but I've make sure it doesn't
            ref._grid_ref_const = new Grid2_ref_xy<T>( const_cast<Grid3<T>&>( g ), x, y, z );
            // Memory leak here counter already in default ctor
            //ref._counter        = new Ref_counter();
            //ref._counter->add_ref();
            return ref;
        }

        // -----------------------------------------------------------------------------

        template<typename T>
        Grid2_const_ref<T>

        Grid2_const_ref<T>::make_xz( const Grid3<T>& g, Range x, int y, Range z )
        {
            Grid2_const_ref<T> ref;
            ref._grid_ref_allocated = true;
            // see 'make_xy()' for the hack
            ref._grid_ref_const = new Grid2_ref_xz<T>( const_cast<Grid3<T>&>( g ), x, y, z );
            // Memory leak here counter already in default ctor
            //ref._counter        = new Ref_counter();
            //ref._counter->add_ref();
            return ref;
        }

        // -----------------------------------------------------------------------------

        template<typename T>
        Grid2_const_ref<T>

        Grid2_const_ref<T>::make_yz( const Grid3<T>& g, int x, Range y, Range z )
        {
            Grid2_const_ref<T> ref;
            ref._grid_ref_allocated = true;
            // see 'make_xy()' for the hack
            ref._grid_ref_const = new Grid2_ref_yz<T>( const_cast<Grid3<T>&>( g ), x, y, z );
            // Memory leak here counter already in default ctor
            //ref._counter        = new Ref_counter();
            //ref._counter->add_ref();
            return ref;
        }

        // -----------------------------------------------------------------------------

        template<typename T>
        Grid2_const_ref<T>::Grid2_const_ref( const Grid2_const_ref<T>& cp ):
            _grid_ref_allocated( cp._grid_ref_allocated ),
            _grid_ref_const( cp._grid_ref_const ),
            _counter( cp._counter )
        {
            _counter->add_ref();
        }

        // -----------------------------------------------------------------------------

        template<typename T>
        Grid2_const_ref<T>::Grid2_const_ref( const Grid2_ref<T>& cp ):
            _grid_ref_allocated( cp._grid_ref_allocated ),
            _grid_ref_const( cp._grid_ref_const ),
            _counter( cp._counter )
        {
            _counter->add_ref();
        }

        // -----------------------------------------------------------------------------

        template<typename T>
        Grid2_const_ref<T>::~Grid2_const_ref( )
        {
            if ( _counter->release() == 0 )
            {
                // When false means the pointer has been allocated
                // externally of Grid2_ref
                if ( _grid_ref_allocated )
                {
                    delete _grid_ref_const;
                }
                delete _counter;
            }
        }

        // -----------------------------------------------------------------------------

        template<typename T>
        const T& Grid2_const_ref<T>::operator()( const Vector2i& pos ) const
        {
            return ( *_grid_ref_const )( Vector2i( pos( 0 ), pos( 1 ) ) );
        }

        // -----------------------------------------------------------------------------

        template<typename T>
        const T& Grid2_const_ref<T>::operator()( int x, int y ) const
        {
            return ( *_grid_ref_const )( x, y );
        }

        // -----------------------------------------------------------------------------

        template<typename T>
        const T& Grid2_const_ref<T>::operator()( const Idx2& idx ) const
        {
            Vector2i v = idx.to_2d();
            return ( *_grid_ref_const )( v(0), v(1) );
        }
    }
}

