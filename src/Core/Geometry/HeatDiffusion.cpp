#include <Core/Geometry/HeatDiffusion.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

Time t( const Scalar& m, const Scalar& h ) {
    return ( m * h * h );
}

void heat( const AreaMatrix& A, const Time& t, const LaplacianMatrix& L,
           Heat& u, const Delta& delta ) {
    Eigen::SimplicialLLT<Math::Sparse> llt;
    llt.compute( ( A + ( t * L ) ) );
    Math::VectorN b = delta;
    u.getMap() = llt.solve( b );
}

Heat heat( const AreaMatrix& A, const Time& t, const LaplacianMatrix& L,
           const Delta& delta ) {
    Heat u( L.rows() );
    Eigen::SimplicialLLT<Math::Sparse> llt;
    llt.compute( ( A + ( t * L ) ) );
    Math::VectorN b = delta;
    u.getMap() = llt.solve( b );
    return u;
}

} // namespace Geometry
} // namespace Core
} // namespace Ra
