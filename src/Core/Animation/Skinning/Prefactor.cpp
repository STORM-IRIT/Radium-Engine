#include "Prefactor.hpp"

namespace Ra {
namespace Core {
namespace Animation {



WeightBlock computeBlockXij( const Vector3& v, const Scalar& w ) {
    const uint blockRow = 3;
    const uint blockClm = 12;
    WeightBlock Xij( blockRow, blockClm );
    Vector4 V = Vector4( v( 0 ), v( 1 ), v( 2 ), 1.0 );
    for( uint j = 0; j < ( blockClm / blockRow ); ++j ) {
        for( uint i = 0; i < blockRow; ++i ) {
            Xij.coeffRef( i, ( i + j ) ) = w * V( j % blockRow );
        }
    }
    return Xij;
}



PrefactorMatrix computeMatrixX( const VectorArray< Vector3 >& v, const MeshWeight& weight, const uint handle_size ) {
    const uint blockRow = 3;
    const uint blockClm = 12;
    PrefactorMatrix X( ( blockRow * v.size() ), ( blockClm * handle_size ) );
    for( uint i = 0; i < v.size(); ++i ) {
        for( auto w : weight[i] ) {
            WeightBlock Xij = computeBlockXij( v[i], w.second );
            for( uint k = 0; k < ( blockClm / blockRow ); ++k ) {
                for( uint j = 0; j < blockRow; ++j ) {
                    X.coeffRef( ( i + j ), ( ( w.first * blockClm ) + ( j + k ) ) ) = Xij.coeff( j, ( j + k ) );
                }
            }
        }
    }
    return X;
}



} // namespace Animation
} // Namespace Core
} // Namespace Ra
