#include <Core/Geometry/PointCloud.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

inline Math::Vector3 meanPoint( const Container::Vector3Array& pts ) {
    return pts.getMap().rowwise().mean();
}

inline Math::Transform principalAxis( const Container::Vector3Array& pts ) {
    Math::Transform result;

    Math::Vector3 meanPt = meanPoint( pts );
    result.translation() = meanPt;

    // Subtract points from their average.
    Container::Vector3Array ptsAvg( pts.size() );
    ptsAvg.getMap() = pts.getMap().colwise() - meanPt;
    CORE_ASSERT( Math::areApproxEqual( meanPoint( ptsAvg ).squaredNorm(), 0.f ), "oops" );

    // Compute variance-covariance matrix
    Math::MatrixN vCov =
        ( 1.f / ( pts.size() - 1 ) ) * ( ptsAvg.getMap() * ptsAvg.getMap().transpose() );

    // Solve eigen vectors
    Eigen::SelfAdjointEigenSolver<Math::Matrix3> solver( vCov );
    result.linear() = solver.eigenvectors();
    return result;
}

Math::Obb pcaObb( const Container::Vector3Array& pts ) {
    Math::Transform pca = principalAxis( pts );
    Math::Transform pcaInv = pca.inverse();

    // Compute the AABB in principal axis space.
    Container::Vector3Array alignedPts;
    alignedPts.reserve( pts.size() );

    // TODO : is there an eigen trick to do that without a loop ?
    for ( const auto& v : pts )
    {
        alignedPts.push_back( pcaInv * v );
    }

    Math::Aabb aligned = aabb( alignedPts );
    return Math::Obb( aligned, pca );
}

Math::Aabb aabb( const Container::Vector3Array& pts ) {
    return pts.size() > 0
               ? Math::Aabb( pts.getMap().rowwise().minCoeff(), pts.getMap().rowwise().maxCoeff() )
               : Math::Aabb();
}

} // namespace Geometry
} // namespace Core
} // namespace Ra
