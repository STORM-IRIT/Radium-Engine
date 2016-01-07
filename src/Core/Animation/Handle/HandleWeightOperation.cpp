#include <Core/Animation/Handle/HandleWeightOperation.hpp>

#include <utility>
#include <Core/Log/Log.hpp>

namespace Ra {
namespace Core {
namespace Animation {



WeightMatrix extractWeightMatrix( const MeshWeight& weight, const uint weight_size ) {
    WeightMatrix W( weight.size(), weight_size );
    W.setZero();
    for( uint i = 0; i < weight.size(); ++i ) {
        for( const auto& w: weight[i] ) {
            W.coeffRef( i, w.first ) = w.second;
        }
    }
    return W;
}



MeshWeight extractMeshWeight( const WeightMatrix& matrix ) {
    MeshWeight W( matrix.rows() );
    for( int i = 0; i < matrix.rows(); ++i ) {
        for( int j = 0; j < matrix.cols(); ++j ) {
            if( matrix.coeff( i, j ) != 0.0 ) {
                std::pair< uint, Scalar > w( uint(j), matrix.coeff( i, j ) );
                W[i].push_back( w );
            }
        }
    }
    return W;
}



uint getMaxWeightIndex( const WeightMatrix& weights, const uint vertexID ) {
    uint maxId = uint( -1 );
    VectorN row = weights.row( vertexID );
    row.maxCoeff(&maxId);
    return maxId;
}



void getMaxWeightIndex( const WeightMatrix& weights, std::vector< uint >& handleID ) {
    handleID.resize( weights.rows() );
    for( int i = 0; i < weights.rows(); ++i ) {
        handleID[i] = getMaxWeightIndex( weights, i );
    }
}



void checkWeightMatrix( const WeightMatrix& matrix, const bool FAIL_ON_ASSERT ) {
    if( check_NAN( matrix, FAIL_ON_ASSERT ) ) {
        LOG( logDEBUG ) << "Matrix is good.";
    } else {
        LOG( logDEBUG ) << "Matrix is not good.";
    }

    if( check_NoWeightVertex( matrix, FAIL_ON_ASSERT ) ) {
        LOG( logDEBUG ) << "Matrix is good.";
    } else {
        LOG( logDEBUG ) << "Matrix is not good.";
    }
}



bool RA_CORE_API check_NAN( const WeightMatrix& matrix, const bool FAIL_ON_ASSERT ) {
    bool status = true;
    LOG( logDEBUG ) << "Searching for nans in the matrix...";
    for( int k = 0; k < matrix.outerSize(); ++k ) {
        for( WeightMatrix::InnerIterator it( matrix, k ); it; ++it ) {
            const uint        i     = it.row();
            const uint        j     = it.col();
            const Scalar      value = it.value();
            const std::string text  = "Element (" + std::to_string( i ) + "," + std::to_string(j) + ") is nan.";
            if( FAIL_ON_ASSERT ) {
                CORE_ASSERT( !isnan( value ), text.c_str() );
            } else {
                if( isnan( value ) ) {
                    LOG( logDEBUG ) << text;
                    status = false;
                }
            }
        }
    }
    return status;
}

bool RA_CORE_API check_NoWeightVertex( const WeightMatrix& matrix, const bool FAIL_ON_ASSERT ) {
    bool status = true;
    LOG( logDEBUG ) << "Searching for empty rows in the matrix...";
    for( int i = 0; i < matrix.rows(); ++i ) {
        Sparse row = matrix.row( i );
        if( row.nonZeros() == 0 ) {
            const std::string text = "Vertex " + std::to_string( i ) + " has no weights.";
            if( FAIL_ON_ASSERT ) {
                CORE_ASSERT( false, text.c_str() );
            } else {
                status = false;
                LOG( logDEBUG ) << text;
            }
        }
    }
    return status;
}



} // namespace Animation
} // Namespace Core
} // Namespace Ra
