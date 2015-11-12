#ifndef GRID2_HPP
#define GRID2_HPP

#include <Core/Containers/Grid3.hpp>
#include <Core/Containers/Grid2_ref.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Index/Idx2.hpp>

namespace Ra
{
    namespace Core
    {    
        /** @class Grid2
          * @brief utility to store/use 2d grids
          * @tparam T : Cell type. Default constructor MUST be defined.
          *
          * TODO: doc usage
          *
          * @warning If you inherit from this class follow the rules:
          * children must keep '_size' and '_pad_off' attributes updated,
          * all virtual functions are to be overriden properly
          */
        template<typename T>
        struct Grid2 : public Grid2_ref<T>
        {
            // implem notes:
            // Warning: don't forget to call Grid2_ref<T>() if you add a new constructor
            // Warning: never use directly the std::vector to copy between grids always
            // use the overloaded accessor ( , ) which can be overriden by a children.
        
            Grid2(const Ra::Core::Vector2i& size_ = Ra::Core::Vector2i::Zero(),
                  const T& val = T(),
                  const Ra::Core::Vector2i& pad = Ra::Core::Vector2i::Zero());
        
            Grid2(const Ra::Core::Vector2i& s,
                  const T* vals,
                  const Ra::Core::Vector2i& pad = Ra::Core::Vector2i::Zero());
        
            Grid2(const Grid2& cp);
        
        
            /// Assign from another reference Grid2_ref -> hard copy
            virtual Grid2<T>& operator=(Grid2_const_ref<T> cp);
        
            /// When assign another Grid2 we do an hard copy
            virtual Grid2<T>& operator= (const Grid2<T>& cp);
        
        
            // -------------------------------------------------------------------------
            /// @name Accessors
            // -------------------------------------------------------------------------
        
            /// Size allocated to store the grid
            Ra::Core::Vector2i alloc_size() const { return _size; }
        
            /// Logical size of the grid using padding offset
            Ra::Core::Vector2i size() const { return _size -_pad_off * 2; }
        
            Ra::Core::Vector2i get_padd_offset() const { return _pad_off; }
        
            const std::vector<T>& get_vals() const { return _vals; }
        
            /// Access linear storage of the grid where:
            /// linear_idx = x  +  (_size.x) * y
            const T* ptr() const { return &(_vals[0]); }
        
            // -------------------------------------------------------------------------
            /// @name Access array element
            /// @note Accessors are declined with different types (const/non-const etc.)
            /// but their imlementations use get_val(),
            /// so you only need to overide the get_val() operator :)
            // -------------------------------------------------------------------------
        
            virtual       T& get_val(int x, int y);
            virtual const T& get_val(int x, int y) const;
        
            /// non-const accessors
            /// @{
            Grid2<T>& operator() (Range x, Range y   );
            T&        operator() (int x, int y       );
            T&        operator() (const Idx2& idx );
            T&        operator() (const Ra::Core::Vector2i& pos);
            /// @}
        
            /// const accessors
            /// @{
            const Grid2<T>& operator() (Range x, Range y    ) const;
            const T&        operator() (int   x, int   y    ) const;
            const T&        operator() ( const Idx2& idx ) const;
            const T&        operator() ( const Ra::Core::Vector2i& pos) const;
            /// @}
        
        private:
            // implem note: its safer if only this class is allowed to allocate its
            // own memory, children should not be allowed to fiddle with it
            void init_vals(const T* vals);
        
            std::vector<T> _vals;    ///< Linear storage of the 3D grid
        protected:
            Ra::Core::Vector2i       _size;
            Ra::Core::Vector2i       _pad_off; ///< padding offsets (nb elts)
        };   
    }
}

#include <Core/Containers/Grid2.inl>

#endif
