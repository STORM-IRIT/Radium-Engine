#ifndef GRID3_HPP
#define GRID3_HPP

#include <vector>
#include <Core/Index/Idx3.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Containers/GridEnums.hpp>

namespace Ra
{
    namespace Core
    {    
        template< typename T>
        struct Grid2_ref;
        
        template< typename T>
        struct Grid2_const_ref;
        
        /** @class Grid3
          * @brief utility to store/use 3d grids with cuda
          * @tparam T : Cell type. Default constructor MUST be defined.
          *
          * TODO: doc usage
          *
          */
        
        //FIXME: handle bool correctly has std::vector won't ...
        template<class T>
        struct Grid3
        {
            /// @brief Padding kind enumerant.
            enum Pad_t {
                COPY,   ///< padding will extend border values
                CUSTOM  ///< padding will initialize to specified value
            };
        
            virtual ~Grid3(){ }
        
            // -------------------------------------------------------------------------
            /// @name Constructors
            // -------------------------------------------------------------------------
        
            /// @param[in] size : 3d size of the grid
            /// @param[in] val : value used to initialize the grid
            /// @param[in] pad :
            Grid3(const Ra::Core::Vector3i& size = Ra::Core::Vector3i::Zero(),
                  const T& val = T(),
                  const Ra::Core::Vector3i& pad = Ra::Core::Vector3i::Zero() );
        
            /// @param[in] x,y,z : 3d size of the grid
            /// @param[in] vals : array of values used to initialize the grid
            /// @param[in] pad :
            Grid3(int x, int y, int z,
                  const T* vals,
                  const Ra::Core::Vector3i& pad = Ra::Core::Vector3i::Zero());
        
            /// @param[in] s : 3d size of the grid
            /// @param[in] vals : array of values used to initialize the grid
            /// @param[in] pad :
            Grid3(const Ra::Core::Vector3i& s,
                  const T* vals,
                  const Ra::Core::Vector3i& pad = Ra::Core::Vector3i::Zero());
        
            /// Copy constructor do a hard copy
            Grid3( const Grid3<T>& g );
        
            /// Concatenate a list of grids, dimensions are computed according to the
            /// maximal dimension allowed 'max' for the final grid
            /// @param list : list of 3d grids to be concatenated. All grids MUST be
            /// equal in size.
            /// @param max : Maximum size of the new grid resulting from
            /// the concatenation of the grid list.
            /// @param out_idx : returned indices in the new grid to access first
            /// non-padded data for each input grid.
            /// The new grid is filled if necessary but not padded.
            Grid3(const std::vector< Grid3<T>* >& list,
                  const Ra::Core::Vector3i& max,
                  std::vector<Idx3>& out_idx);
        
            /// Symetric padding of the grid.
            /// @param padding : the number of faces added to each side of the grid.
            /// @param type : How do we fill the new faces? (user defined value,
            /// repeat values by extruding each orinal face etc.)
            /// @param val : padding value when (type == CUSTOM). Otherwise parameter
            /// is ignored
            void padd(int padding, Pad_t type = COPY, const T& val = T() );
        
        
            // -------------------------------------------------------------------------
            /// @name Accessors
            // -------------------------------------------------------------------------
        
            /// Size allocated to store the grid
            Ra::Core::Vector3i alloc_size() const { return _size; }
        
            /// Logical size of the grid using padding offset
            Ra::Core::Vector3i size() const { return _size - _pad_off * 2; }
        
            Ra::Core::Vector3i get_padd_offset() const { return _pad_off; }
        
            // TODO: these expose too much the internal storage of the class and should
            // be remove -> use data() instead for linear access
            //{
            const std::vector<T>& get_vals() const { return _vals; }
        
            /// Access linear storage of the grid where:
            /// linear_idx = x  +  (_size.x) * y  +  (_size.x * _size.y) * z
            const T* ptr() const { return &(_vals[0]); }
            //}
        
            /// Assignement op do a hard copy
            Grid3<T>& operator= (const Grid3<T>& cp);
        
            /// Use this if you need linear access instead of 3d access
            /// linear_idx = x  +  (_size.x) * y  +  (_size.x * _size.y) * z
                  T& data(int linear_idx)       { return _vals[linear_idx]; }
            const T& data(int linear_idx) const { return _vals[linear_idx]; }
        
            // -------------------------------------------------------------------------
            /// @name 3D Accessors
            // -------------------------------------------------------------------------
        
            T& operator() (const Ra::Core::Vector3i& coord);
            T& operator() (int x, int y, int z);
            T& operator() (const Idx3& idx);
        
            const T& operator() (const Ra::Core::Vector3i& coord) const;
            const T& operator() (int x, int y, int z) const;
            const T& operator() (const Idx3& idx) const;
        
            // -------------------------------------------------------------------------
            /// @name 2D Accessors
            // -------------------------------------------------------------------------
            /// @defgroup Acces a 2D xy slice
            // Note: Implemented in grid2_cu.inl because of cross definitions
            /// @{
            Grid2_ref<T> operator() (Range x, Range y, int   z);
            Grid2_ref<T> operator() (Range x, int   y, Range z);
            Grid2_ref<T> operator() (int   x, Range y, Range z);
        
            const Grid2_const_ref<T> operator() (Range x, Range y, int   z) const;
            const Grid2_const_ref<T> operator() (Range x, int   y, Range z) const;
            const Grid2_const_ref<T> operator() (int   x, Range y, Range z) const;
            /// @}
        
        private:
            void init_vals(const T* vals);
        
            Ra::Core::Vector3i _size;        ///< 3d size of the grid (nb elts)
        
            // TODO: I think it was an error to including _pad_off in the grid it should be handled by the user.
            // We should get rid of it in future implementations
            Ra::Core::Vector3i _pad_off;     ///< padding offsets (nb elts)
            std::vector<T> _vals;  ///< Linear storage of the 3D grid
        };    
    }
}

#include <Core/Containers/Grid3.inl>

#endif
