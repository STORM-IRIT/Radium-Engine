#ifndef RADIUMENGINE_GRID2_CONST_REF_HPP__
#define RADIUMENGINE_GRID2_CONST_REF_HPP__

#include <Core/RaCore.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Index/Idx2.hpp>

namespace Ra
{
    namespace Core
    {
        template <typename T, uint D>
        class Grid;

        template<typename T>
        struct Grid2_ref;

        struct Range
        {
            Range() : _dyn_range( true ), _a( 0 ), _b( -1 ) { }

            //Range(const Range& r) : _dyn_range(r._dyn_range), _a(r._a), _b(r._b) { }

            /// @param start, end: staring index and end index (both included)
            Range( int start, int end ) : _dyn_range( false ), _a( start ), _b( end + 1 ) { }

            int nb_elts() const
            {
                return _b - _a;
            }

            /// If false you should not consider _a and _b but the size of the object
            bool _dyn_range;

            int _a; ///< starting index (included)
            int _b; ///< last index (excluded)
        };

        /// @brief select the whole span of the grid
        struct All : public Range
        {
            All() : Range()
            {
            }
        };

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

            Grid2_const_ref( const Grid<T, 2>& g );

            static Grid2_const_ref<T> make_xy( const Grid<T, 3>& g, Range x, Range y, int   z );
            static Grid2_const_ref<T> make_xz( const Grid<T, 3>& g, Range x, int   y, Range z );
            static Grid2_const_ref<T> make_yz( const Grid<T, 3>& g, int   x, Range y, Range z );

            Grid2_const_ref( const Grid2_const_ref<T>& cp );
            Grid2_const_ref( const Grid2_ref<T>& cp );

            /// Only destroy if never referenced in another shallow copy of Grid2_ref_const
            virtual ~Grid2_const_ref( );

            /// Logical size of the grid using padding offset
            Vector2i size()            const
            {
                return _grid_ref_const->sizeVector() /*- _grid_ref_const->get_padd_offset() * 2*/;
            }
            Vector2i alloc_size()      const
            {
                return _grid_ref_const->alloc_size();
            }
            //Vector2i get_padd_offset() const { return _grid_ref_const->get_padd_offset();                               }

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
            const Grid<T, 2>* _grid_ref_const;

            /// Count the number of shallow copy of this ref
            Ref_counter* _counter;
        };
    }
}

#include <Core/Containers/Grid2_const_ref.inl>

#endif
