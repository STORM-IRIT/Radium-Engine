#include <Core/Animation/StretchableTwistableBoneSkinning.hpp>

#include <Core/Animation/Skeleton.hpp>
#include <Core/Animation/SkinningData.hpp>
#include <Core/Geometry/DistanceQueries.hpp>

namespace Ra {
namespace Core {
namespace Animation {

WeightMatrix computeSTBS_weights( const Vector3Array& inMesh,
                                  const Skeleton& skel ) {
    std::vector<Eigen::Triplet<Scalar>> triplets;
    for ( int i = 0; i < int( inMesh.size() ); ++i )
    {
        const auto& pi = inMesh[uint( i )];
        for ( int j = 0; j < int( skel.size() ); ++j )
        {
            Vector3 a, b;
            skel.getBonePoints( uint( j ), a, b );
            const Vector3 ab = b - a;
            Scalar t         = Geometry::projectOnSegment( pi, a, ab );
            if ( t > 0 ) { triplets.push_back( Eigen::Triplet<Scalar>( i, j, t ) ); }
        }
    }
    WeightMatrix weights( uint( inMesh.size() ), skel.size() );
    weights.setFromTriplets( triplets.begin(), triplets.end() );
    return weights;
}

void linearBlendSkinningSTBS( const SkinningRefData& refData,
                              const Vector3Array& tangents,
                              const Vector3Array& bitangents,
                              const Skeleton& poseSkel,
                              SkinningFrameData& frameData ) {
    // first decompose all pose transforms
    const auto& pose = frameData.m_refToCurrentRelPose;
    Ra::Core::VectorArray<Matrix3> R( pose.size() );
    Matrix3 S; // we don't mind it
#pragma omp parallel for
    for ( int i = 0; i < int( pose.size() ); ++i )
    {
        pose[i].computeRotationScaling( &R[i], &S );
    }
    // then go through vertices
    const auto& restSkel = refData.m_skeleton;
    const auto& vertices = refData.m_referenceMesh.vertices();
    const auto& normals = refData.m_referenceMesh.normals();
#pragma omp parallel for
    for ( int i=0; i < frameData.m_currentPosition.size(); ++i )
    {
        frameData.m_currentPosition[i] = Vector3::Zero();
        frameData.m_currentNormal[i] = Vector3::Zero();
        frameData.m_currentTangent[i] = Vector3::Zero();
        frameData.m_currentBitangent[i] = Vector3::Zero();
    }
    for ( int k = 0; k < refData.m_weights.outerSize(); ++k )
    {
        const int nonZero = refData.m_weights.col( k ).nonZeros();
        WeightMatrix::InnerIterator it0( refData.m_weights, k );
#pragma omp parallel for
        for ( int nz = 0; nz < nonZero; ++nz )
        {
            WeightMatrix::InnerIterator it = it0 + Eigen::Index( nz );
            const uint i                   = it.row();
            const uint j                   = it.col();
            const Scalar w                 = it.value();
            Vector3 a, b, a_, b_;
            restSkel.getBonePoints( j, a, b );
            poseSkel.getBonePoints( j, a_, b_ );
            Vector3 es = ( std::sqrt( ( b_ - a_ ).squaredNorm() / ( b - a ).squaredNorm() ) - 1 ) *
                         ( b - a );
            const Scalar eis = refData.m_weightSTBS.coeff( i, j );
            frameData.m_currentPosition[i] += w * ( a_ + R[j] * ( eis * es - a + vertices[i] ) );
            frameData.m_currentNormal[i] += w * R[j] * normals[i];
            frameData.m_currentTangent[i] += w * R[j] * tangents[i];
            frameData.m_currentBitangent[i] += w * R[j] * bitangents[i];
        }
    }
}

DQList computeDQSTBS( const Pose& relPose,
                      const Skeleton& poseSkel,
                      const Skeleton& restSkel,
                      const WeightMatrix& weight,
                      const WeightMatrix& weightSTBS ) {
    CORE_ASSERT( ( relPose.size() == size_t( weight.cols() ) ),
                 "pose/weight size mismatch." );
    DQList DQ( uint( weight.rows() ),
               DualQuaternion( Quaternion( 0, 0, 0, 0 ), Quaternion( 0, 0, 0, 0 ) ) );
    std::cout << DQ.size() << std::endl;
    // Stores the first non-zero quaternion for each vertex.
    std::vector<uint> firstNonZero( weight.rows(), std::numeric_limits<uint>::max() );
    // Contains the converted dual quaternions from the pose
    std::vector<DualQuaternion> poseDQ( relPose.size() );
    // first decompose all pose transforms
    VectorArray<Matrix3> R( relPose.size() );
    VectorArray<Vector3> A( relPose.size() );
    VectorArray<Vector3> A_( relPose.size() );
    VectorArray<Vector3> Si( relPose.size() );
    Matrix3 S; // we don't mind it
#pragma omp parallel for
    for ( int i = 0; i < int( relPose.size() ); ++i )
    {
        relPose[i].computeRotationScaling( &R[i], &S );
        Ra::Core::Vector3 b, b_;
        restSkel.getBonePoints( i, A[i], b );
        poseSkel.getBonePoints( i, A_[i], b_ );
        Si[i] = ( std::sqrt( ( b_ - A_[i] ).squaredNorm() / ( b - A[i] ).squaredNorm() ) - 1 ) *
                ( b - A[i] );
    }
    // Loop through all transforms Tj
    for ( int k = 0; k < weight.outerSize(); ++k )
    {
        poseDQ[k] = DualQuaternion( relPose[k] );
        // Count how many vertices are influenced by the given transform
        const int nonZero = weight.col( k ).nonZeros();
        WeightMatrix::InnerIterator it0( weight, k );
#pragma omp parallel for
        // This for loop is here just because OpenMP wants classic for loops.
        // Since we cannot iterate directly through the non-zero elements using the
        // InnerIterator, we initialize an InnerIterator to the first element and
        // then we increase it nz times.
        /*
         * This crappy piece of code was done in order to avoid the critical section
         *           DQ[i] += wq;
         *
         * that was occurring when parallelizing the main for loop.
         *
         * NOTE: this could be definitely improved by using std::thread
         */
        // Loop through all vertices vi who depend on Tj
        for ( int nz = 0; nz < nonZero; ++nz )
        {
            WeightMatrix::InnerIterator itn = it0 + Eigen::Index( nz );
            const uint i                    = itn.row();
            const uint j                    = itn.col();
            const Scalar w                  = itn.value();
            Transform T( R[j] );
            T.translation() = A_[j] + R[j] * ( weightSTBS.coeff( i, j ) * Si[j] - A[j] );
            auto D          = DualQuaternion( T );
            D.normalize();
            firstNonZero[i] = std::min( firstNonZero[i], uint( k ) );
            const Scalar sign =
                Ra::Core::Math::signNZ( poseDQ[j].getQ0().dot( poseDQ[firstNonZero[i]].getQ0() ) );
            const auto wq = D * w * sign;
            DQ[i] += wq;
        }
    }
    // Normalize all dual quats.
#pragma omp parallel for
    for ( int i = 0; i < int( DQ.size() ); ++i )
    {
        DQ[i].normalize();
    }
    return DQ;
}

void RA_CORE_API dualQuaternionSkinningSTBS( const SkinningRefData& refData,
                                             const Vector3Array& tangents,
                                             const Vector3Array& bitangents,
                                             const Skeleton& poseSkel,
                                             SkinningFrameData& frameData ) {
    const auto DQ = computeDQSTBS( frameData.m_refToCurrentRelPose,
                                   poseSkel, refData.m_skeleton,
                                   refData.m_weights,
                                   refData.m_weightSTBS );
    const auto& vertices = refData.m_referenceMesh.vertices();
    const auto& normals = refData.m_referenceMesh.normals();
#pragma omp parallel for
    for ( int i = 0; i < frameData.m_currentPosition.size(); ++i )
    {
        frameData.m_currentPosition[i] = DQ[i].transform( vertices[i] );
        frameData.m_currentNormal[i] = DQ[i].rotate( normals[i] );
        frameData.m_currentTangent[i] = DQ[i].rotate( tangents[i] );
        frameData.m_currentBitangent[i] = DQ[i].rotate( bitangents[i] );
    }
}

} // namespace Animation
} // namespace Core
} // namespace Ra
