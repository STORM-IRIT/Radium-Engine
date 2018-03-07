#include <Core/Algorithm/Diffusion/Diffusion.hpp>

#include <utility>

namespace Ra {
namespace Core {
namespace Algorithm {

ScalarValue diffuseDelta( const Geometry::AdjacencyMatrix& A, const Delta& delta,
                          const Scalar lambda, const uint iteration ) {
    ScalarValue u( delta );
    ScalarValue tmp( delta );
    unsigned int iter = iteration;
    iter = iter + ( iter % 2 );
    for ( uint n = 0; n < iter; ++n )
    {
        for ( Delta::InnerIterator u_it( u, 0 ); u_it; ++u_it )
        {
            Scalar oneRingMeanValue = 0.0;
            for ( Geometry::AdjacencyMatrix::InnerIterator A_it( A, u_it.row() ); A_it; ++A_it )
            {
                oneRingMeanValue += u.coeff( A_it.row(), 0 );
            }
            oneRingMeanValue /= (Scalar)A.col( u_it.row() ).nonZeros();
            tmp.coeffRef( u_it.row(), 0 ) =
                ( lambda * oneRingMeanValue ) + ( delta.coeff( u_it.row(), 0 ) * ( 1.0 - lambda ) );
        }
        u.swap( tmp );
    }
    return u;
}

void diffuseDelta( const Geometry::AdjacencyMatrix& A, const Delta& delta, const Scalar lambda,
                   const uint iteration, ScalarValue& value ) {
    ScalarValue u = delta;
    ScalarValue tmp = delta;
    uint iter = iteration;
    iter = iter + ( iter % 2 ); // Why always even?
    for ( uint n = 0; n < iter; ++n )
    {

        //        for( Delta::InnerIterator u_it( u, 0 ); u_it; ++u_it) {
        //            uint j = u_it.row();
        //            const Scalar uv = u_it.value();
        //            Scalar oneRingMeanValue = 0.0;
        //            uint nonZero = 0;
        //            for( Geometry::AdjacencyMatrix::InnerIterator A_it( A, j ); A_it; ++A_it) {
        //                uint i = A_it.row();
        //                oneRingMeanValue += u.coeff( i, 0 );
        //                ++nonZero;
        //            }

        //            oneRingMeanValue /= ( Scalar )nonZero; //oneRingMeanValue /= ( Scalar )A.col(
        //            j ).nonZeros(); tmp.coeffRef( j, 0 ) = ( lambda * oneRingMeanValue ) + ( uv *
        //            ( 1.0 - lambda ) );
        //        }
        //        u.swap( tmp );

#pragma omp parallel for
        for ( int k = 0; k < A.outerSize(); ++k )
        {
            uint nonZero = 0;
            Scalar oneRingMeanValue = 0.0;
            for ( Geometry::AdjacencyMatrix::InnerIterator it( A, k ); it; ++it )
            {
                const uint i = it.row();
                ++nonZero;
                oneRingMeanValue += u.coeff( i, 0 );
            }
            if ( nonZero > 0 )
            {
                oneRingMeanValue /= (Scalar)nonZero;
                Scalar s =
                    ( lambda * oneRingMeanValue ) + ( delta.coeff( k, 0 ) * ( 1.0 - lambda ) );
#pragma omp critical
                { tmp.coeffRef( k, 0 ) = s; }
            }
        }
#pragma omp barrier
        std::swap( u, tmp ); // u.swap( tmp );
    }
    std::swap( u, value ); // u.swap( value );
}

} // namespace Algorithm
} // namespace Core
} // namespace Ra
