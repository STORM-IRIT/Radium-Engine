#include <Core/Animation/Skinning/RotationCenterSkinning.hpp>

namespace Ra {
namespace Core {
namespace Animation {

Scalar weightSimilarity( const Eigen::SparseVector<Scalar>& v1w,
                         const Eigen::SparseVector<Scalar>& v2w, Scalar sigma ) {

    const Scalar sigmaSq = sigma * sigma;

    Scalar result = 0;
    // Iterating over non zero coefficients
    for ( Eigen::SparseVector<Scalar>::InnerIterator it1( v1w ); it1; ++it1 )
    {
        const uint j = it1.index();
        const Scalar& W1j = it1.value();              // This one is necessarily non zero
        const Scalar& W2j = v2w.coeff( it1.index() ); // This one may be 0.

        if ( W2j > 0 )
        {
            for ( Eigen::SparseVector<Scalar>::InnerIterator it2( v2w ); it2; ++it2 )
            {
                const uint k = it2.index();
                const Scalar& W1k = v1w.coeff( it2.index() );
                const Scalar& W2k = it2.value();
                if ( j != k && W1k > 0 )
                {
                    const Scalar diff =
                        std::exp( -Math::ipow<2>( ( W1j * W2k ) - ( W1k * W2j ) ) / ( sigmaSq ) );
                    result += W1j * W1k * W2j * W2k * diff;
                }
            }
        }
    }
    return result;
}

void computeCoR( Skinning::RefData& dataInOut, Scalar sigma, Scalar weightEpsilon ) {
    LOG( logDEBUG ) << "Precomputing CoRs";

    // First step : subdivide the original mesh until weights are sufficiently close enough.
    Scalar maxWeightDistance = 0.f;
    TriangleMesh subdividedMesh = dataInOut.m_referenceMesh;

    // Store the weights as row major here because we are going to query the per-vertex weights.
    Eigen::SparseMatrix<Scalar, Eigen::RowMajor> subdividedWeights = dataInOut.m_weights;

    // Convert the mesh to DCEL for easy processing.
    Dcel dcel;
    convert( subdividedMesh, dcel );

    // The mesh will be subdivided by repeated edge-split, so that adjacent vertices
    // weights are distant of at most `weightEpsilon`.
    // New vertices created by the edge splitting and their new weights are computed
    // and appended to the existing vertices.
    do
    {
        maxWeightDistance = 0;

        // Stores the edges to split
        std::vector<Index> edgesToSplit;

        // Compute all weights distances for all edges.
        for ( const auto& edge : dcel.m_fulledge )
        {
            Vertex_ptr v1 = edge->V( 0 );
            Vertex_ptr v2 = edge->V( 1 );

            Scalar weightDistance =
                ( subdividedWeights.row( v1->idx ) - subdividedWeights.row( v2->idx ) ).norm();

            maxWeightDistance = std::max( maxWeightDistance, weightDistance );
            if ( weightDistance > weightEpsilon )
            {
                edgesToSplit.push_back( edge->idx );
            }
        }

        LOG( logDEBUG ) << "Max weight distance is " << maxWeightDistance;

        // We found some edges over the limit, so we split them.
        if ( !edgesToSplit.empty() )
        {
            LOG( logDEBUG ) << "Splitting " << edgesToSplit.size() << " edges";
            int startIndex = subdividedWeights.rows();
            int numCols = subdividedWeights.cols();

            Eigen::SparseMatrix<Scalar, Eigen::RowMajor> newWeights(
                startIndex + edgesToSplit.size(), numCols );

            newWeights.topRows( startIndex ) = subdividedWeights;
            subdividedWeights = newWeights;

            int i = 0;

            // Split ALL the edges !
            for ( const auto& edge : edgesToSplit )
            {
                int V1Idx = dcel.m_fulledge[edge]->V( 0 )->idx;
                int V2Idx = dcel.m_fulledge[edge]->V( 1 )->idx;

                DcelOperations::splitEdge( dcel, edge, 0.5f );
                subdividedWeights.row( startIndex + i ) =
                    0.5f * ( subdividedWeights.row( V1Idx ) + subdividedWeights.row( V2Idx ) );
                ++i;
            }
        }
        edgesToSplit.clear();

    } while ( maxWeightDistance > weightEpsilon );

    // get the subdivided mesh back into mesh form.
    convert( dcel, subdividedMesh );

    // Second step : evaluate the integrals over all triangles for all vertices.
    CORE_ASSERT( subdividedMesh.vertices().size() == subdividedWeights.rows(),
                 "Weights and vertices don't match" );

    dataInOut.m_CoR.clear();
    dataInOut.m_CoR.reserve( dataInOut.m_referenceMesh.vertices().size() );

    const uint nVerts = dataInOut.m_referenceMesh.vertices().size();
    // naive implementation : iterate over all the triangles (of the subdivided mesh)
    // for all vertices (of the original mesh).
    for ( uint i = 0; i < nVerts; ++i )
    {

        // Check that the first vertices of the subdivided mesh have not changed.
        ON_ASSERT( const Vector3& p = dataInOut.m_referenceMesh.vertices()[i] );
        CORE_ASSERT( subdividedMesh.vertices()[i] == p, "Inconsistency in the meshes" );

        Vector3 cor( 0, 0, 0 );
        Scalar sumweight = 0;
        const Eigen::SparseVector<Scalar> Wi = subdividedWeights.row( i );

        // Sum the cor and weights over all triangles of the subdivided mesh.
        for ( uint t = 0; t < subdividedMesh.m_triangles.size(); ++t )
        {
            const Triangle& tri = subdividedMesh.m_triangles[t];
            std::array<Vector3, 3> triVerts;
            MeshUtils::getTriangleVertices( subdividedMesh, t, triVerts );

            const Scalar area = MeshUtils::getTriangleArea( subdividedMesh, t );
            const Eigen::SparseVector<Scalar> triWeight =
                ( 1 / 3.f ) * ( subdividedWeights.row( tri[0] ) + subdividedWeights.row( tri[1] ) +
                                subdividedWeights.row( tri[2] ) );
            const Vector3 centroid = ( triVerts[0] + triVerts[1] + triVerts[2] ) / 3.f;

            const Scalar s = weightSimilarity( Wi, triWeight, sigma );

            cor += s * area * centroid;
            sumweight += s * area;
        }

        // Avoid division by 0
        if ( sumweight > 0 )
        {
            dataInOut.m_CoR.push_back( ( 1.f / sumweight ) * cor );
        } else
        { dataInOut.m_CoR.push_back( Vector3::Zero() ); }

#if defined CORE_DEBUG
        if ( i % 100 == 0 )
        {
            LOG( logDEBUG ) << "CoR:" << i << " / " << nVerts;
        }
#endif // CORE_DEBUG
    }
}

void corSkinning( const Vector3Array& input, const Animation::Pose& pose,
                  const Animation::WeightMatrix& weight, const Vector3Array& CoR,
                  Vector3Array& output ) {
    const uint size = input.size();
    output.resize( size );

    CORE_ASSERT( CoR.size() == size, "Invalid center of rotations" );

    // Compute the dual quaternions
    AlignedStdVector<DualQuaternion> DQ;
    Animation::computeDQ( pose, weight, DQ );

    // Do LBS on the COR with weights of their associated vertices
    Vector3Array transformedCoR;
    Animation::linearBlendSkinning( CoR, pose, weight, transformedCoR );
#pragma omp parallel for
    for ( int i = 0; i < int( size ); ++i )
    {
        output[i] = DQ[i].rotate( input[i] - CoR[i] ) + transformedCoR[i];
    }
}
} // namespace Animation
} // namespace Core
} // namespace Ra
