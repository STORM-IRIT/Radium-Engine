#include <Core/Animation/Handle/HandleWeightOperation.hpp>

#include <utility>

namespace Ra {
namespace Core {
namespace Animation {



WeightMatrix extractWeightMatrix( const MeshWeight& weight, const uint weight_size ) {
    WeightMatrix W( weight.size(), weight_size );
    W.setZero();
    for( uint i = 0; i < weight.size(); ++i ) {
        for( auto w : weight[i] ) {
            W.coeffRef( i, w.first ) = w.second;
        }
    }
    return W;
}

MeshWeight extractMeshWeight( const WeightMatrix& matrix ) {
    MeshWeight W( matrix.rows() );
    for( uint i = 0; i < matrix.rows(); ++i ) {
        for( uint j = 0; j < matrix.cols(); ++j ) {
            if( matrix.coeff( i, j ) != 0.0 ) {
                std::pair< uint, Scalar > w( j, matrix.coeff( i, j ) );
                W[i].push_back( w );
            }
        }
    }
    return W;
}



} // namespace Animation
} // Namespace Core
} // Namespace Ra
