#include <Core/Containers/Grid2_ref.hpp>
#include <Core/Containers/Grid.hpp>

namespace Ra
{
    namespace Core
    {
        template< typename T>
        Grid2_ref<T>::Grid2_ref() :
            Grid2_const_ref<T>(),
            _grid_ref(0)
        {
        }

        // -----------------------------------------------------------------------------

        template< typename T>
        Grid2_ref<T>::Grid2_ref( Grid<T, 2>& g) :
            Grid2_const_ref<T>( g ),
            _grid_ref( &g )
        {

        }

        // -----------------------------------------------------------------------------

        template< typename T>
        Grid2_ref<T>

        Grid2_ref<T>::make_xy(Grid<T, 3>& g, Range x, Range y, int z) {
            // TODO: factor code (just pass new Grid2_ref_xy<T>( g, z); as a func parameter)
            Grid2_ref<T> ref;
            ref._grid_ref_allocated = true;
            ref._grid_ref       = new Grid2_ref_xy<T>(g, x, y, z);
            ref._grid_ref_const = ref._grid_ref;
            // Memory leak here counter already in default ctor
            //ref._counter        = new /*typename Grid2_const_ref<T>::*/Ref_counter();
            //ref._counter->add_ref();
            return ref;
        }

        // -----------------------------------------------------------------------------

        template< typename T>
        Grid2_ref<T>

        Grid2_ref<T>::make_xz(Grid<T, 3>& g, Range x, int y, Range z) {
            // TODO: factor code (just pass new Grid2_ref_xy<T>( g, z); as a func parameter)
            Grid2_ref<T> ref;
            ref._grid_ref_allocated = true;
            ref._grid_ref       = new Grid2_ref_xz<T>(g, x, y, z);
            ref._grid_ref_const = ref._grid_ref;
            // Memory leak here counter already in default ctor
            //ref._counter        = new /*typename Grid2_const_ref<T>::*/Ref_counter();
            //ref._counter->add_ref();
            return ref;
        }

        // -----------------------------------------------------------------------------

        template< typename T>
        Grid2_ref<T>

        Grid2_ref<T>::make_yz(Grid<T, 3>& g, int x, Range y, Range z) {
            // TODO: factor code (just pass new Grid2_ref_xy<T>( g, z); as a func parameter)
            Grid2_ref<T> ref;
            ref._grid_ref_allocated = true;
            ref._grid_ref       = new Grid2_ref_yz<T>(g, x, y, z);
            ref._grid_ref_const = ref._grid_ref;
            // Memory leak here counter already in default ctor
            //ref._counter        = new /*typename Grid2_const_ref<T>::*/Ref_counter();
            //ref._counter->add_ref();
            return ref;
        }

        // -----------------------------------------------------------------------------

        template< typename T>
        Grid2_ref<T>::Grid2_ref( const Grid2_ref<T>& cp ) :
            Grid2_const_ref<T>( cp ),
            _grid_ref( cp._grid_ref )
        {

        }

        // -----------------------------------------------------------------------------

        template< typename T>
        void Grid2_ref<T>::copy_from( const Grid2_const_ref<T>& cp )
        {
            // Check reference integrity, if it fails check that a children
            // inehriting from this class has not forgoten to call Grid2_ref()
            // constructor...
            assert(cp._grid_ref_const != 0);
            assert(cp._counter        != 0);

            if(this != &cp) // Avoid self assignment
            {
                #if 0
                //note: I Keep this piece of code as an example to do a shallow copy

                // Check if we have to release old ref
                if(_counter->release() == 0)
                {
                    if( _grid_ref_allocated ) delete _grid_ref;
                    delete _counter;
                }
                _grid_ref_allocated = cp._grid_ref_allocated;
                _grid_ref = cp._grid_ref;
                _counter  = cp._counter;
                _counter->add_ref();
                #else
                // Grids must have the same size. Resizing them is not feasible
                // in every cases (i.e. 2d grids might use a 3d grid behind the hood
                // what to do then to resize the 3d instance?).
                // Of course we could have a virtual method resize() which fails
                // when necessary but it adds too much complexity to the already
                // complex architecture. Let's keep it simple
                assert(_grid_ref->alloc_size() == cp.alloc_size());

                // TODO: a good optim would be to cast 'cp' and '_grid_ref' to their
                // specialized instance type -> it would avoid the performance hit
                // of calling a virtual at each iteration and trade it for a two nested
                // switch. For large data it can be beneficial.
                for( Idx2 idx(cp.alloc_size(), 0); idx.is_in(); ++idx )
                    (*this)( idx ) = cp( idx );
                #endif
            }
        }

        // -----------------------------------------------------------------------------

        template< typename T>
        Grid<T, 2>&

        Grid2_ref<T>::operator= (Grid2_const_ref<T> cp)
        {
            copy_from( cp );
            return *_grid_ref;
        }

        // -----------------------------------------------------------------------------

        template< typename T>
        Grid<T, 2>&

        Grid2_ref<T>::operator= (Grid2_ref<T> cp)
        {
            copy_from( cp );
            return *_grid_ref;
        }

        // -----------------------------------------------------------------------------

        template< typename T>
        const T& Grid2_ref<T>::operator() (int x, int y) const {
            return (*Grid2_const_ref<T>::_grid_ref_const)(x, y);
        }

        // -----------------------------------------------------------------------------

        template< typename T>
        const T& Grid2_ref<T>::operator() (const Idx2& idx) const {
            Vector2i v = idx.to_2d();
            return (*Grid2_const_ref<T>::_grid_ref_const)(v.x, v.y);
        }

        // -----------------------------------------------------------------------------

        template< typename T>
        const T& Grid2_ref<T>::operator() (const Vector2i& pos) const {
            return (*_grid_ref)(pos.x, pos.y);
        }


        // -----------------------------------------------------------------------------

        template< typename T>
        T& Grid2_ref<T>::operator() (int x, int y) {
            return (*_grid_ref).at(Vector2i(x, y));
        }

        // -----------------------------------------------------------------------------

        template< typename T>
        T& Grid2_ref<T>::operator() (const Idx2& idx) {
            Vector2i v = idx.to_2d();
            return (*_grid_ref).at(Vector2i(v(0), v(1)));
        }

        // -----------------------------------------------------------------------------

        template< typename T>
        T& Grid2_ref<T>::operator() (const Vector2i& pos) {
            return (*_grid_ref)(pos(0), pos(1));
        }

        // -----------------------------------------------------------------------------
    }
}

