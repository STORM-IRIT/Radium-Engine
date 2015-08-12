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

        #if 0
        /*
            For the moment I want to keep an example of the old implementation of
            Grid2_ref. At this time there was no Grid2_ref_const implementation
        */

        template<typename T>
        struct Grid2_ref {

            Grid2_ref();

            Grid2_ref( Grid<T, 2>& g);

            static Grid2_ref<T> make_xy( Grid<T, 3>& g, int z);
            static Grid2_ref<T> make_xz( Grid<T, 3>& g, int y);
            static Grid2_ref<T> make_yz( Grid<T, 3>& g, int x);

            Grid2_ref( const Grid2_ref<T>& cp );

            /// Only destroy if never referenced in another shallow copy of Grid2_ref
            virtual ~Grid2_ref( );

            /// Assignement is an hard copy (as we would expect from a Grid2& reference)
            Grid<T, 2>& operator= (const Grid2_ref<T>& cp);


            Vector2i alloc_size()      const { return _grid_ref->alloc_size();      }
            Vector2i get_padd_offset() const { return _grid_ref->get_padd_offset(); }

            // -------------------------------------------------------------------------
            // Access referenced data:
            // -------------------------------------------------------------------------

            const T& operator() (int x, int y) const { return (*_grid_ref)(x, y); }

            const T& operator() (const Idx2& idx) const {
                Vector2i v = idx.to_2d();
                return (*_grid_ref)(v.x, v.y);
            }

            // Non const accessors
            // @{
            T& operator() (int x, int y) { return (*_grid_ref)(x, y); }

            T& operator() (const Idx2& idx) {
                Vector2i v = idx.to_2d();
                return (*_grid_ref)(v.x, v.y);
            }
            // @}

        private:
            /// @brief counts how many instances points to the same '_grid_ref' pointer
            struct Ref_counter {
            public:
                Ref_counter() : _count(0) { }
                void add_ref() { _count++; }
                int  release() { return --_count; }
            private:
                int _count; // Reference counter
            };

            bool _grid_ref_allocated; ///< Wether we have internally allocated _grid_ref
            Grid<T, 2>* _grid_ref;   ///< Polymorphic pointer to any 2D grid
            Ref_counter* _counter;    ///< Count the number of shallow copy of this ref
        };

        // -----------------------------------------------------------------------------
        // Implemens of Grid2_ref
        // -----------------------------------------------------------------------------

        template< typename T>
        Grid2_ref<T>::Grid2_ref() : _grid_ref(0), _counter(0) {
            _grid_ref_allocated = false;
            _counter  = new Ref_counter();
            _counter->add_ref();
        }

        // -----------------------------------------------------------------------------

        template< typename T>
        Grid2_ref<T>::Grid2_ref( Grid<T, 2>& g){
            _grid_ref_allocated = false;
            _grid_ref = &g;
            _counter  = new Ref_counter();
            _counter->add_ref();
        }

        // -----------------------------------------------------------------------------

        template< typename T>
        Grid2_ref<T>

        Grid2_ref<T>::make_xy( Grid<T, 3>& g, int z) {
            Grid2_ref<T> ref;
            ref._grid_ref_allocated = true;
            ref._grid_ref = new Grid2_ref_xy<T>(g, z);
            // Memory leak here counter already in default ctor
            //ref._counter  = new Ref_counter();
            //ref._counter->add_ref();
            return ref;
        }

        // -----------------------------------------------------------------------------

        template< typename T>
        Grid2_ref<T>

        Grid2_ref<T>::make_xz( Grid<T, 3>& g, int y) {
            Grid2_ref<T> ref;
            ref._grid_ref_allocated = true;
            ref._grid_ref = new Grid2_ref_xz<T>(g, y);
            // Memory leak here counter already in default ctor
            //ref._counter  = new Ref_counter();
            //ref._counter->add_ref();
            return ref;
        }

        // -----------------------------------------------------------------------------

        template< typename T>
        Grid2_ref<T>

        Grid2_ref<T>::make_yz( Grid<T, 3>& g, int x) {
            Grid2_ref<T> ref;
            ref._grid_ref_allocated = true;
            ref._grid_ref = new Grid2_ref_yz<T>(g, x);
            // Memory leak here counter already in default ctor
            //ref._counter  = new Ref_counter();
            //ref._counter->add_ref();
            return ref;
        }

        // -----------------------------------------------------------------------------

        template< typename T>
        Grid2_ref<T>::Grid2_ref( const Grid2_ref<T>& cp ):
            _grid_ref_allocated( cp._grid_ref_allocated ),
            _grid_ref( cp._grid_ref ),
            _counter( cp._counter )
        {
            _counter->add_ref();
        }

        // -----------------------------------------------------------------------------

        template< typename T>
        Grid2_ref<T>::~Grid2_ref( )
        {
            if(_counter->release() == 0)
            {
                // When false means the pointer has been allocated
                // externally of Grid2_ref
                if( _grid_ref_allocated ) delete _grid_ref;
                delete _counter;
            }
        }

        // -----------------------------------------------------------------------------

        template< typename T>
        Grid<T, 2>&

        Grid2_ref<T>::operator= (const Grid2_ref<T>& cp)
        {
            // Check reference integrity, if it fails check that a children
            // inehriting from this class has not forgoten to call Grid2_ref()
            // constructor...
            assert(cp._grid_ref != 0);
            assert(cp._counter  != 0);

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
                // when necessary but it adds to much complexity to the already
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
            return *_grid_ref;
        }
        #endif
    }
}

