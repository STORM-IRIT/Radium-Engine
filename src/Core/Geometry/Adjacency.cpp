#include <Core/Geometry/Adjacency.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

/////////////////////
/// GLOBAL MATRIX ///
/////////////////////

// //////////////// //
// ADJACENCY MATRIX //
// //////////////// //

AdjacencyMatrix uniformAdjacency( const uint point_size, const Container::VectorArray<Triangle>& T ) {
    AdjacencyMatrix A( point_size, point_size );
    for ( const auto& t : T )
    {
        uint i = t( 0 );
        uint j = t( 1 );
        uint k = t( 2 );
        A.coeffRef( i, j ) = 1;
        A.coeffRef( j, k ) = 1;
        A.coeffRef( k, i ) = 1;
    }
    return A;
}

AdjacencyMatrix uniformAdjacency( const Container::VectorArray<Math::Vector3>& p, const Container::VectorArray<Triangle>& T ) {
    AdjacencyMatrix A( p.size(), p.size() );
    for ( const auto& t : T )
    {
        uint i = t( 0 );
        uint j = t( 1 );
        uint k = t( 2 );
        A.coeffRef( i, j ) = 1;
        A.coeffRef( j, k ) = 1;
        A.coeffRef( k, i ) = 1;
    }
    return A;
}

void uniformAdjacency( const Container::VectorArray<Math::Vector3>& p, const Container::VectorArray<Triangle>& T,
                       AdjacencyMatrix& Adj ) {
    Adj.resize( p.size(), p.size() );
#pragma omp parallel for
    for ( int n = 0; n < int( T.size() ); ++n )
    {
        const Triangle& t = T[n];
        const uint i = t( 0 );
        const uint j = t( 1 );
        const uint k = t( 2 );
#pragma omp critical
        {
            Adj.coeffRef( i, j ) = 1;
            Adj.coeffRef( j, k ) = 1;
            Adj.coeffRef( k, i ) = 1;
        }
    }
}

TVAdj triangleUniformAdjacency( const Container::VectorArray<Math::Vector3>& p, const Container::VectorArray<Triangle>& T ) {
    const uint p_size = p.size();
    const uint t_size = T.size();
    TVAdj A( t_size, p_size );
    for ( uint t = 0; t < t_size; ++t )
    {
        const uint i = T[t]( 0 );
        const uint j = T[t]( 1 );
        const uint k = T[t]( 2 );
        A.coeffRef( t, i ) = 1;
        A.coeffRef( t, j ) = 1;
        A.coeffRef( t, k ) = 1;
    }
    return A;
}

AdjacencyMatrix cotangentWeightAdjacency( const Container::VectorArray<Math::Vector3>& p,
                                          const Container::VectorArray<Triangle>& T ) {
    AdjacencyMatrix A( p.size(), p.size() );
    for ( const auto& t : T )
    {
        uint i = t( 0 );
        uint j = t( 1 );
        uint k = t( 2 );
        Math::Vector3 IJ = p[j] - p[i];
        Math::Vector3 JK = p[k] - p[j];
        Math::Vector3 KI = p[i] - p[k];
        Scalar cotI = Math::Vector::cotan( IJ, ( -KI ).eval() );
        Scalar cotJ = Math::Vector::cotan( JK, ( -IJ ).eval() );
        Scalar cotK = Math::Vector::cotan( KI, ( -JK ).eval() );
        A.coeffRef( i, j ) += cotK;
        A.coeffRef( j, i ) += cotK;
        A.coeffRef( j, k ) += cotI;
        A.coeffRef( k, j ) += cotI;
        A.coeffRef( k, i ) += cotJ;
        A.coeffRef( i, k ) += cotJ;
    }
    return ( 0.5 * A );
}

// ///////////// //
// DEGREE MATRIX //
// ///////////// //

DegreeMatrix adjacencyDegree( const AdjacencyMatrix& A ) {
    DegreeMatrix D( A.rows(), A.cols() );
    D.reserve( A.rows() );
    for ( int i = 0; i < D.diagonal().size(); ++i )
    {
        D.coeffRef( i, i ) = A.row( i ).sum();
    }
    return D;
}

} // namespace Geometry
} // namespace Core
} // namespace Ra
