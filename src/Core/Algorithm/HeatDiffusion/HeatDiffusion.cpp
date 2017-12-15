#include <Core/Algorithm/HeatDiffusion/HeatDiffusion.hpp>

namespace Ra {
namespace Core {
namespace Algorithm {



Time t( const Scalar& m,
        const Scalar& h ) {
    return ( m * h * h );
}



void heat( const Geometry::AreaMatrix&      A,
           const Time&                      t,
           const Geometry::LaplacianMatrix& L,
           Heat&                            u,
           const Delta&                     delta ) {
    Eigen::SimplicialLLT< Sparse > llt;
    llt.compute( ( A + ( t * L ) ) );
    VectorN b = delta;
    u.getMap() = llt.solve( b );
}



Heat heat( const Geometry::AreaMatrix&      A,
           const Time&                      t,
           const Geometry::LaplacianMatrix& L,
           const Delta&                     delta ) {
    Heat u( L.rows() );
    Eigen::SimplicialLLT< Sparse > llt;
    llt.compute( ( A + ( t * L ) ) );
    VectorN b = delta;
    u.getMap() = llt.solve( b );
    return u;
}



} // namespace Algorithm
} // namespace Core
} // namespace Ra
