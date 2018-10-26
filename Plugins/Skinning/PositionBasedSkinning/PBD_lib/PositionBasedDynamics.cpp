#include "PositionBasedDynamics.h"

using namespace PBD;

const Scalar eps = (Scalar)1e-6;

//////////////////////////////////////////////////////////////////////////
// PositionBasedDynamics
//////////////////////////////////////////////////////////////////////////

bool PositionBasedDynamics::solve_DistanceConstraint(
    const Ra::Core::Vector3& p0, Scalar invMass0, const Ra::Core::Vector3& p1, Scalar invMass1,
    const Scalar restLength, const Scalar compressionStiffness, const Scalar stretchStiffness,
    Ra::Core::Vector3& corr0, Ra::Core::Vector3& corr1 ) {
    Scalar wSum = invMass0 + invMass1;
    if ( wSum == 0.0 )
        return false;

    Ra::Core::Vector3 n = p1 - p0;
    Scalar d = n.norm();
    n.normalize();

    Ra::Core::Vector3 corr;
    if ( d < restLength )
    {
        corr = compressionStiffness * n * ( d - restLength ) / wSum;
    } else
    { corr = stretchStiffness * n * ( d - restLength ) / wSum; }

    corr0 = invMass0 * corr;
    corr1 = -invMass1 * corr;
    return true;
}

bool PositionBasedDynamics::solve_BindConstraint(
    const Ra::Core::Vector3& particle, const Ra::Core::Vector3& startBone,
    const Ra::Core::Vector3& endBone, Scalar invMassParticle, const Scalar restLength,
    const Scalar compressionStiffness, const Scalar stretchStiffness, Ra::Core::Vector3& corr ) {

    if ( invMassParticle == 0.0 )
        return false;

    Ra::Core::Vector3 projectionBone = projectionPointSegment( particle, startBone, endBone );
    Ra::Core::Vector3 n = projectionBone - particle;
    Scalar d = n.norm();
    n.normalize();

    if ( d < restLength )
    {
        corr = compressionStiffness * n * ( d - restLength ) * 0.5;
    } else
    { corr = stretchStiffness * n * ( d - restLength ) * 0.5; }

    return true;
}

bool PositionBasedDynamics::solve_VolumeConstraint(
    const Ra::Core::Vector3& p0, Scalar invMass0, const Ra::Core::Vector3& p1, Scalar invMass1,
    const Ra::Core::Vector3& p2, Scalar invMass2, const Ra::Core::Vector3& p3, Scalar invMass3,
    const Scalar restVolume, const Scalar negVolumeStiffness, const Scalar posVolumeStiffness,
    Ra::Core::Vector3& corr0, Ra::Core::Vector3& corr1, Ra::Core::Vector3& corr2,
    Ra::Core::Vector3& corr3 ) {
    /*	Vector3r d1 = p1 - p0;
        Vector3r d2 = p2 - p0;
        Vector3r d3 = p3 - p0;*/
    Scalar volume = 1.0f / 6.0f * ( p1 - p0 ).cross( p2 - p0 ).dot( p3 - p0 );

    corr0.setZero();
    corr1.setZero();
    corr2.setZero();
    corr3.setZero();

    if ( posVolumeStiffness == 0.0 && volume > 0.0 )
        return false;

    if ( negVolumeStiffness == 0.0 && volume < 0.0 )
        return false;

    Ra::Core::Vector3 grad0 = ( p1 - p2 ).cross( p3 - p2 );
    Ra::Core::Vector3 grad1 = ( p2 - p0 ).cross( p3 - p0 );
    Ra::Core::Vector3 grad2 = ( p0 - p1 ).cross( p3 - p1 );
    Ra::Core::Vector3 grad3 = ( p1 - p0 ).cross( p2 - p0 );

    Scalar lambda = invMass0 * grad0.squaredNorm() + invMass1 * grad1.squaredNorm() +
                    invMass2 * grad2.squaredNorm() + invMass3 * grad3.squaredNorm();

    if ( fabs( lambda ) < eps )
        return false;

    if ( volume < 0.0 )
    {
        lambda = negVolumeStiffness * ( volume - restVolume ) / lambda;
    } else
    { lambda = posVolumeStiffness * ( volume - restVolume ) / lambda; }

    corr0 = -lambda * invMass0 * grad0;
    corr1 = -lambda * invMass1 * grad1;
    corr2 = -lambda * invMass2 * grad2;
    corr3 = -lambda * invMass3 * grad3;

    return true;
}

Ra::Core::Vector3
PositionBasedDynamics::projectionPointSegment( const Ra::Core::Vector3& point,
                                               const Ra::Core::Vector3& startSegment,
                                               const Ra::Core::Vector3& endSegment ) {
    Ra::Core::Vector3 projection;
    Ra::Core::Vector3 segment = endSegment - startSegment;
    Ra::Core::Vector3 startSegmentPoint = point - startSegment;

    const Scalar normSegment = segment.squaredNorm();
    if ( normSegment == 0.0 )
    {
        projection = startSegment;
    } else
    {
        Scalar dotSegmentPoint = startSegmentPoint.dot( segment );
        Scalar t = dotSegmentPoint / normSegment;
        t = std::max( 0.0f, std::min( 1.0f, t ) );
        projection = startSegment + t * segment;
    }
    return projection;
}
