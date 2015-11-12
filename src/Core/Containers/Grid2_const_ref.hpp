#ifndef GRID2_CONST_REF_HPP
#define GRID2_CONST_REF_HPP

//#include <Core/RaCore.hpp>
#include <Core/Containers/GridEnums.hpp>
#include <Core/Index/Idx2.hpp>
#include <Core/Math/LinearAlgebra.hpp>
//#include <Core/Containers/Grid3.hpp>

namespace Ra
{
    namespace Core
    {
        template <typename T>
        class Grid2;

        template<typename T>
        struct Grid2_ref;

        /**
         * @name Grid2_const_ref
         * @brief Class encapsulating a constant pointer of a Grid2 (like a smart pointer)
         *
         * Constant version of a Grid2_ref.
         * To avoid duplicating doc go see #Grid2_ref docs.
         *
         */
        template<typename T>
        struct Grid2_const_ref
        {
            Grid2_const_ref( const Grid2<T>& g );

            static Grid2_const_ref<T> make_xy( const Grid3<T>& g, Range x, Range y, int   z );
            static Grid2_const_ref<T> make_xz( const Grid3<T>& g, Range x, int   y, Range z );
            static Grid2_const_ref<T> make_yz( const Grid3<T>& g, int   x, Range y, Range z );

            Grid2_const_ref( const Grid2_const_ref<T>& cp );
            Grid2_const_ref( const Grid2_ref<T>& cp );

            /// Only destroy if never referenced in another shallow copy of Grid2_ref_const
            virtual ~Grid2_const_ref( );

            /// Logical size of the grid using padding offset
            Vector2i size()            const
            {
                return _grid_ref_const->size() /*- _grid_ref_const->get_padd_offset() * 2*/;
            }
            Vector2i alloc_size()      const
            {
                return _grid_ref_const->alloc_size();
            }
            Vector2i get_padd_offset() const { return _grid_ref_const->get_padd_offset(); }

            // -------------------------------------------------------------------------
            // Access referenced data:
            // -------------------------------------------------------------------------

            const T& operator()( int x, int y )        const;
            const T& operator()( const Idx2& idx )  const;
            const T& operator()( const Vector2i& pos ) const;

        protected:
            template <typename B>
            friend struct Grid2_ref;

            /// Don't make sense to have a default constructor for a reference
            /// However we need it internally
            Grid2_const_ref();

            /// @brief counts how many instances points to the same '_grid_ref' pointer
            struct Ref_counter
            {
            public:
                Ref_counter() : _count( 0 ) { }
                void add_ref()
                {
                    _count++;
                }
                int  release()
                {
                    return --_count;
                }
            private:
                int _count; // Reference counter
            };

            /// Wether we have internally allocated _grid_ref
            bool _grid_ref_allocated;

            /// Polymorphic pointer to any 2D grid. This is const because the class
            /// is supposed to represent a const reference. Do not change it
            const Grid2<T>* _grid_ref_const;

            /// Count the number of shallow copy of this ref
            Ref_counter* _counter;
        };
    }
}

#include <Core/Containers/Grid2_const_ref.inl>

#endif
