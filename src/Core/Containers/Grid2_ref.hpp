#ifndef GRID2_REF_HPP
#define GRID2_REF_HPP

//#include <Core/RaCore.hpp>
#include <Core/Containers/Grid2_const_ref.hpp>

namespace Ra
{
    namespace Core
    {
//        template <typename T>
//        class Grid2;

        /**
         * @name Grid2_ref
         * @brief Class encapsulating a pointer of a Grid2 (like a smart pointer)
         *
         * This smart pointer like class for 2D grids should be seen as a reference
         * to a polymorphic 2D grid #Grid2. Unlike smart pointers we don't overload
         * '*' or '->'. Instead we overload the usuals accesors of a Grid2
         * (for instance access a grid element with operator (x,y) ).
         * Therefore the class is more like a "smart reference".
         *
         * You can build a reference from any 2D grid that inherit from #Grid2
         * @code
         * // Below passing a Grid2 argument or a children as parameter of 'fun()'
         * // will only copy pointers
         * template<typename T>
         * void fun( Grid2_ref<T> g_ref ){ g_ref(x,y) = value; }
         *
         * {
         *     Grid2 <T> grid2;
         *     Grid2_ref<T> a_ref( grid2 );
         *     fun( grid2 ); // No extra copy of grid2
         *     fun( a_ref ); // Do the same as the previous line
         * }
         * @endcode
         *
         * But what's the point of the class, we could just use 'Grid2&'?
         * Actually the class hide allocation/deletion of memory in this case:
         * @code
         * // Example of a standard way to handle class polymorphism
         * template<typename T>
         * void foobar( Grid2<T>& g ){ g(x,y) = value; }
         *
         * {
         *    Grid2_ref_xy<T>* g_ptr = new Grid2_ref_xy<T>( ... );
         *    foobar( *g_ptr ); // specific behavior
         *    delete g_ptr;
         * }
         *
         * // Example with Grid2_ref which doesn't need memory handling
         * template<typename T>
         * void foobar2( Grid2_ref<T> g ){ g(x,y) = value; }
         *
         * {
         *    // This constructor internally do a 'new Grid2_ref_xy<T>( ... );'
         *    Grid2_ref<T> g_ref( grid3, 5);
         *    foobar( g_ptr ); // specific behavior
         *    // deletion is done automatically in Grid2_ref destructor
         * }
         * @endcode
         *
         * If you need a "constant smart reference" use Grid2_const_ref which is
         * the same as Grid2_ref but guarantees not to change what is referenced.
         * Alternatively you can also use a 'const Grid2_ref&' which is equivalent to
         * a 'Grid2_const_ref'.
         *
         * @warning: child inheriting from this class should not forget to call
         * Grid2_ref<T>() constructor. Otherwise you are likely to segfault if a child
         * is passed as an argument of type Grid2_ref/Grid2_const_ref
         *
         * @warning: Prefer to pass directly by value Grid2_ref/Grid2_const_ref.
         * You can't use types like 'Grid2_ref&' or 'Grid2_const_ref&'. First it's
         * uselless as the copy constructor of these are very lightweight. Second it
         * will prevent to use many functions returning  Grid2_ref/Grid2_const_ref by
         * value (in C++ you cannot assign to a non-const reference a temporary value).
         * You could use 'const Grid2_const_ref&' which is also equivalent to
         * 'const Grid2_ref&'.
         *
         */
        template<typename T>
        struct Grid2_ref : public Grid2_const_ref<T>
        {
            Grid2_ref( Grid2<T>& g );

            static Grid2_ref<T> make_xy( Grid3<T>& g, Range x, Range y, int   z );
            static Grid2_ref<T> make_xz( Grid3<T>& g, Range x, int   y, Range z );
            static Grid2_ref<T> make_yz( Grid3<T>& g, int   x, Range y, Range z );

            Grid2_ref( const Grid2_ref<T>& cp );

            /// Only destroy if never referenced in another shallow copy of Grid2_ref
            virtual ~Grid2_ref( ) { }

            /// Assignement is an hard copy (copy every values of the grid as we would
            /// expect from a Grid2& reference)
            Grid2<T>& operator= ( Grid2_const_ref<T> cp );
            Grid2<T>& operator= ( Grid2_ref<T>       cp );

            // -------------------------------------------------------------------------
            // Access referenced data:
            // -------------------------------------------------------------------------

            /// @defgroup Non const accessors
            /// @{
            T& operator()( int x, int y );
            T& operator()( const Idx2& idx );
            T& operator()( const Vector2i& pos );
            /// @}

            /// @defgroup Const accessors
            // Even though we should inherit them from Grid2_const_ref we have to
            // re-define them as they get hidden by the non-const versions. Yes its
            // another C++ pitfall...
            /// @{
            const T& operator()( int x, int y )        const;
            const T& operator()( const Idx2& idx )  const;
            const T& operator()( const Vector2i& pos ) const;
            /// @}

        private:
            /// Don't make sense to have a default constructor for a reference
            /// However we need it internally
            Grid2_ref();

            /// Factor code when assign to another reference const and non const
            void copy_from( const Grid2_const_ref<T>& cp );

            /// Polymorphic pointer to any 2D grid
            Grid2<T>* _grid_ref;
        };
    }
}

#include <Core/Containers/Grid2_ref.inl>

#endif
