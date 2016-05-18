#include <Core/Animation/Handle/HandleWeightOperation.hpp>

#include <utility>
#include <Core/Log/Log.hpp>

namespace Ra {
namespace Core {
namespace Animation {


WeightMatrix extractWeightMatrix(const MeshWeight& weight, const uint weight_size)
{
    WeightMatrix W(weight.size(), weight_size);
    W.setZero();
    for (uint i = 0; i < weight.size(); ++i)
    {
        for (const auto& w : weight[i])
        {
            W.coeffRef(i, w.first) = w.second;
        }
    }
    return W;
}


MeshWeight extractMeshWeight(const WeightMatrix& matrix)
{
    MeshWeight W(matrix.rows());
    for (int i = 0; i < matrix.rows(); ++i)
    {
        for (int j = 0; j < matrix.cols(); ++j)
        {
            if (matrix.coeff(i, j) != 0.0)
            {
                std::pair<uint, Scalar> w(uint(j), matrix.coeff(i, j));
                W[i].push_back(w);
            }
        }
    }
    return W;
}



WeightMatrix partitionOfUnity( const WeightMatrix& weights ) {
    WeightMatrix W( weights.rows(), weights.cols() );
    W.reserve( weights.nonZeros() );
    Ra::Core::Vector1Array norm( weights.rows() );
    #pragma omp parallel for
    for( uint i = 0; i < weights.rows(); ++i ) {
        Ra::Core::VectorN row = weights.row( i );
        norm[i] = row.lpNorm<1>();
        norm[i] = ( norm[i] == 0.0 ) ? 1.0 : norm[i];
    }
    #pragma omp parallel for
    for( uint k = 0; k < weights.outerSize(); ++k ) {
        for( WeightMatrix::InnerIterator it( weights, k ); it; ++it ) {
            const uint   i = it.row();
            const uint   j = it.col();
            const Scalar w = it.value();
            W.coeffRef( i, j ) = w / norm[i];
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



void checkWeightMatrix( const WeightMatrix& matrix, const bool FAIL_ON_ASSERT, const bool MT ) {
    if( check_NAN( matrix, FAIL_ON_ASSERT, MT ) ) {
        LOG( logDEBUG ) << "Matrix is good.";
    } else {
        LOG( logDEBUG ) << "Matrix is not good.";
    }

    if( check_NoWeightVertex( matrix, FAIL_ON_ASSERT, MT ) ) {
        LOG( logDEBUG ) << "Matrix is good.";
    } else {
        LOG( logDEBUG ) << "Matrix is not good.";
    }
}



bool RA_CORE_API check_NAN( const WeightMatrix& matrix, const bool FAIL_ON_ASSERT, const bool MT ) {
    bool status = true;
    LOG( logDEBUG ) << "Searching for nans in the matrix...";
    if( MT ) {
        #pragma omp parallel for
        for( int k = 0; k < matrix.outerSize(); ++k ) {
            for( WeightMatrix::InnerIterator it( matrix, k ); it; ++it ) {
                const Scalar      value = it.value();
                const bool        check = isnan( value );
                #pragma omp atomic
                status &= check;
            }
        }
        if( !status ) {
            if( FAIL_ON_ASSERT ) {
                CORE_ASSERT( status, "At least an element is nan" );
            } else {
                LOG( logDEBUG ) << "At least an element is nan";
            }
        }
    } else {
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
    }
    return status;
}

bool RA_CORE_API check_NoWeightVertex( const WeightMatrix& matrix, const bool FAIL_ON_ASSERT, const bool MT ) {
    bool status = true;
    LOG( logDEBUG ) << "Searching for empty rows in the matrix...";
    if( MT ) {
        #pragma omp parallel for
        for( int i = 0; i < matrix.rows(); ++i ) {
            Sparse row = matrix.row( i );
            const bool check = ( row.nonZeros() == 0 );
            #pragma omp atomic
            status &= check;
        }
        if( !status ) {
            if( FAIL_ON_ASSERT ) {
                CORE_ASSERT( status, "At least a vertex as no weights" );
            } else {
                LOG( logDEBUG ) << "At least a vertex as no weights";
            }
        }
    } else {
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
    }
    return status;
}


} // namespace Animation
} // Namespace Core
} // Namespace Ra
