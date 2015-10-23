#include <Core/Algorithm/Diffusion/Diffusion.hpp>

#include <utility>

namespace Ra {
namespace Core {
namespace Algorithm {



ScalarValue diffuseDelta( const Geometry::AdjacencyMatrix& A, const Delta& delta, const Scalar lambda, const uint iteration ) {
    ScalarValue u( delta );
    ScalarValue tmp( delta );
    unsigned int iter = iteration;
    iter = iter + ( iter % 2 );
    for( uint n = 0; n < iter; ++n ) {
        for( Delta::InnerIterator u_it( u, 0 ); u_it; ++u_it) {
            Scalar oneRingMeanValue = 0.0;
            for( Geometry::AdjacencyMatrix::InnerIterator A_it( A, u_it.row() ); A_it; ++A_it) {
                oneRingMeanValue += u.coeff( A_it.row(), 0 );
            }
            oneRingMeanValue /= ( Scalar )A.col( u_it.row() ).nonZeros();
            tmp.coeffRef( u_it.row(), 0 ) = ( lambda * oneRingMeanValue ) + ( delta.coeff( u_it.row(), 0 ) * ( 1.0 - lambda ) );
        }
        u.swap( tmp );
    }
    return u;
}



void diffuseDelta( const Geometry::AdjacencyMatrix& A, const Delta& delta, const Scalar lambda, const uint iteration, ScalarValue& value ) {
    ScalarValue u( delta );
    ScalarValue tmp( delta );
    unsigned int iter = iteration;
    iter = iter + ( iter % 2 );
    for( uint n = 0; n < iter; ++n ) {
        for( Delta::InnerIterator u_it( u, 0 ); u_it; ++u_it) {
            Scalar oneRingMeanValue = 0.0;
            for( Geometry::AdjacencyMatrix::InnerIterator A_it( A, u_it.row() ); A_it; ++A_it) {
                oneRingMeanValue += u.coeff( A_it.row(), 0 );
            }
            oneRingMeanValue /= ( Scalar )A.col( u_it.row() ).nonZeros();
            tmp.coeffRef( u_it.row(), 0 ) = ( lambda * oneRingMeanValue ) + ( delta.coeff( u_it.row(), 0 ) * ( 1.0 - lambda ) );
        }
        u.swap( tmp );
    }
    u.swap( value );
}


} // namespace Algorithm
} // namespace Core
} // namespace Ra
