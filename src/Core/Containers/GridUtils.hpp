#ifndef GRID_UTILS_HPP
#define GRID_UTILS_HPP

#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Containers/Grid3.hpp>
#include <Core/Containers/Grid2.hpp>

namespace Ra
{
    namespace Core
    {
        namespace GridUtils
        {
            /// Compute 2d gradients with central finite difference.
            /// Borders of the grid are untouched.
            /// @param scale : if 'vals' represent a 2D function what is the range of (x, y)
            /// variables? Scale defines that x ranges to [0; scale.x] and y ranges to
            /// [0; scale.y].
            /// @tparam Real  : a real number type
            /// @tparam Grad2 : gradient must define '.x' and '.y' attributes
            template<typename Real, typename Grad2>
            void grad_2d( Grid2_ref<Real> vals,
                          Grid2_ref<Grad2> grads,
                          const Vector2 scale = Vector2( 1., 1. ) )
            {
                assert( vals.size() == grads.size() );
        
                Real step_x = scale( 0 ) / ( vals.size()( 0 ) - 1 );
                Real step_y = scale( 1 ) / ( vals.size()( 1 ) - 1 );
        
                Idx2 off( vals.size(), 1, 1 );
                for ( Idx2 sub_idx( vals.size() - Vector2i( 2, 2 ), 0 ); sub_idx.is_in(); ++sub_idx )
                {
                    Idx2 idx = off + sub_idx.to_2d();
        
                    Real x_plus_h  = vals( idx + Vector2i( 1, 0 ) );
                    Real x_minus_h = vals( idx + Vector2i( -1, 0 ) );
                    Real grad_x = ( x_plus_h - x_minus_h ) / ( ( Real )2 * step_x );
        
                    Real y_plus_h  = vals( idx + Vector2i( 0,  1 ) );
                    Real y_minus_h = vals( idx + Vector2i( 0, -1 ) );
                    Real grad_y = ( y_plus_h - y_minus_h ) / ( ( Real )2 * step_y );
        
                    grads( idx )( 0 ) = grad_x;
                    grads( idx )( 1 ) = grad_y;
                }
            }
            
            // Test not sure its correct
            template< typename T>
            void diffuse_biharmonic(Grid2_ref<T> grid,
                                    Grid2_const_ref<int> mask,
                                    int nb_iter = 128*16)
            {
                assert( grid.size() == mask.size());
                Ra::Core::Vector2i neighs[4] = {Ra::Core::Vector2i( 1, 0),
                                   Ra::Core::Vector2i( 0, 1),
                                   Ra::Core::Vector2i(-1, 0),
                                   Ra::Core::Vector2i( 0,-1)};
            
                // First we store every grid elements that need to be diffused according
                // to 'mask'
                int mask_nb_elts = mask.size().prod();
                std::vector<int> idx_list;
                idx_list.reserve( mask_nb_elts );
                for(Idx2 idx(mask.size(), 0); idx.is_in(); ++idx) {
                    if( mask( idx ) == 1 )
                        idx_list.push_back( idx.to_linear() );
                }
            
                // Diffuse values with a Gauss-seidel like scheme
                for(int j = 0; j < nb_iter; ++j)
                {
                    // Look up grid elements to be diffused
                    for(unsigned i = 0; i < idx_list.size(); ++i)
                    {
                        Idx2 idx(grid.size(), idx_list[i]);
                        T sum(0.);
                        int nb_neigh = 0;
                        for(int i = 0; i < 4; ++i)
                        {
                            Idx2 idx_neigh = idx + neighs[i];
                            if( idx_neigh.is_in() /*check if inside grid*/ )
                            {
            
                                T val(0.);
                                {
                                    int card = 0;
                                    for(int j = 0; j < 4; ++j)
                                    {
                                        Idx2 idx_2nd_neigh = idx_neigh + neighs[j];
                                        if( idx_2nd_neigh.is_in() /*check if inside grid*/ )
                                        {
                                            val = val + grid( idx_2nd_neigh );
                                            card++;
                                        }
                                    }
            
                                    val = val / (T)( card );
                                }
            
                                sum = sum + val;
                                nb_neigh++;
                            }
                        }
                        sum = sum / (T)( nb_neigh );
                        grid( idx ) = sum;
                    }
                }
            }
            
            // -----------------------------------------------------------------------------
            
            template< typename T>
            void mirror_anti_diag_lower( Grid2_ref<T> grid )
            {
                for (int j = 0; j < grid.size().y; ++j) {
                    for (int i = j+1; i < grid.size().x; ++i) {
                        grid(j, i) = grid(i, j);
                    }
                }
            }
            
            // -----------------------------------------------------------------------------
            
            /// @note @li Diagonale goes from upper left to lower right
            /// @li anti-diagonale goes from lower left to upper right.
            enum Mirror_diag_t {
                LOWER,      ///< Mirror strict lower triangle (formed by the diagonal)
                UPPER,      ///< Mirror strict upper triangle (formed by the diagonal)
                ANTI_UPPER, ///< Mirror strict upper triangle (formed by the anti-diagonal)
                ANTI_LOWER  ///< Mirror strict lower triangle (formed by the anti-diagonal)
            };
            
            template< typename T>
            void mirror_diag(Grid2_ref<T> grid, Mirror_diag_t type)
            {
                // Must be a square grid
                assert( grid.size().x == grid.size().y);
                switch(type){
                case ANTI_LOWER: mirror_anti_diag_lower( grid ); break;
                default:
                    assert(false); // Not implemented
                }
            }
        }
    }
}
#endif
