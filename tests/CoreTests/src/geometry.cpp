#include <Core/Geometry/DistanceQueries.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Math/Math.hpp>
#include <Tests.hpp>

namespace Ra {
namespace Testing {

void run() {
    using Ra::Core::Vector3;
    using Ra::Core::Geometry::FlagsInternal;
    using Ra::Core::Geometry::pointToLineSq;
    using Ra::Core::Geometry::pointToSegmentSq;
    using Ra::Core::Geometry::PointToTriangleOutput;
    using Ra::Core::Geometry::pointToTriSq;
    using Ra::Core::Math::areApproxEqual;

    Vector3 a( 1_ra, 2.3_ra, 4.5_ra );
    Vector3 b( -6_ra, 7_ra, 8.9_ra );
    Vector3 c( -3_ra, 12.3_ra, -42.1_ra );

    // Midpoint.
    Vector3 m = 0.5_ra * ( a + b );

    // Point on the line, before A
    Vector3 na = a - 12_ra * ( b - a );

    // Point on the line after B
    Vector3 nb = a + 42_ra * ( b - a );

    Vector3 y, z;
    Ra::Core::Math::getOrthogonalVectors( ( b - a ).normalized(), y, z );

    // Test line queries.
    RA_VERIFY( areApproxEqual( pointToLineSq( a, a, b - a ), 0_ra ),
               "distance from A to AB" );
    RA_VERIFY( areApproxEqual( pointToLineSq( b, a, b - a ), 0_ra ),
               "distance from B to AB" );
    RA_VERIFY( areApproxEqual( pointToLineSq( na, a, b - a ), 0_ra ),
               "point on the line." );
    RA_VERIFY( areApproxEqual( pointToLineSq( nb, a, b - a ), 0_ra ),
               "point on the line." );
    RA_VERIFY( areApproxEqual( pointToLineSq( m + y, a, b - a ), y.squaredNorm() ),
               "point perpendicular to segment." );

    // Test segment queries
    RA_VERIFY( areApproxEqual( pointToSegmentSq( a, a, b - a ), 0_ra ),
               "Segment extremity" );
    RA_VERIFY( areApproxEqual( pointToSegmentSq( b, a, b - a ), 0_ra ),
               "Segment extremity" );
    RA_VERIFY( areApproxEqual( pointToSegmentSq( na, a, b - a ), ( na - a ).squaredNorm() ),
               "point on the line." );
    RA_VERIFY( areApproxEqual( pointToSegmentSq( nb, a, b - a ), ( nb - b ).squaredNorm() ),
               "point on the line." );
    RA_VERIFY( areApproxEqual( pointToSegmentSq( m + y, a, b - a ), y.squaredNorm() ),
               "point perpendicular to segment." );

    // Test triangle queries

    // Test that each vertex returns itself

    auto da = pointToTriSq( a, a, b, c );
    RA_VERIFY( areApproxEqual( da.distanceSquared, 0_ra ), "distance from A to ABC" );
    RA_VERIFY( da.meshPoint == a, "distance from A to ABC" );
    RA_VERIFY( da.flags == FlagsInternal::HIT_A, "distance from A to ABC" );

    auto db = pointToTriSq( b, a, b, c );
    RA_VERIFY( areApproxEqual( db.distanceSquared, 0_ra ), "distance from B to ABC" );
    RA_VERIFY( db.meshPoint == b, "distance from B to ABC" );
    RA_VERIFY( db.flags == FlagsInternal::HIT_B, "distance from B to ABC" );

    auto dc = pointToTriSq( c, a, b, c );
    RA_VERIFY( areApproxEqual( dc.distanceSquared, 0_ra ), "distance from C to ABC" );
    RA_VERIFY( dc.meshPoint == c, "distance from C to ABC" );
    RA_VERIFY( dc.flags == FlagsInternal::HIT_C, "distance from C to ABC" );

    // Test midpoints of edges
    Vector3 mab = .5_ra * ( a + b );
    Vector3 mac = .5_ra * ( a + c );
    Vector3 mbc = .5_ra * ( b + c );

    auto dmab = pointToTriSq( mab, a, b, c );
    RA_VERIFY( areApproxEqual( dmab.distanceSquared, 0_ra ),
               "Distance from AB midpoint to ABC" );
    RA_VERIFY( dmab.meshPoint.isApprox( mab ), "Distance from AB midpoint to ABC" );
    RA_VERIFY( dmab.flags == FlagsInternal::HIT_AB, "Distance from AB midpoint to ABC" );

    auto dmac = pointToTriSq( mac, a, b, c );
    RA_VERIFY( areApproxEqual( dmac.distanceSquared, 0_ra ),
               "Distance from AC midpoint to ABC" );
    RA_VERIFY( dmac.meshPoint.isApprox( mac ), "Distance from AC midpoint to ABC" );
    RA_VERIFY( dmac.flags == FlagsInternal::HIT_CA, "Distance from AC midpoint to ABC" );

    auto dmbc = pointToTriSq( mbc, a, b, c );
    RA_VERIFY( areApproxEqual( dmbc.distanceSquared, 0_ra ),
               "Distance from BC midpoint to ABC" );
    RA_VERIFY( dmbc.meshPoint.isApprox( mbc ), "Distance from BC midpoint to ACC" );
    RA_VERIFY( dmbc.flags == FlagsInternal::HIT_BC, "Distance from BC midpoint to ACC" );

    // Point inside the triangle
    Vector3 g = ( 1_ra / 3_ra ) * ( a + b + c );

    auto dg = pointToTriSq( g, a, b, c );
    RA_VERIFY( areApproxEqual( dg.distanceSquared, 0_ra ),
               "Distance from centroid to ABC" );
    RA_VERIFY( dg.meshPoint.isApprox( g ), "Distance from centroid to ABC" );
    RA_VERIFY( dg.flags == FlagsInternal::HIT_FACE, "Distance from centroid to ABC" );
}
} // namespace Testing
} // namespace Ra

int main( int argc, const char** argv ) {
    using namespace Ra;

    if ( !Testing::init_testing( 1, argv ) )
    {
        return EXIT_FAILURE;
    }

#pragma omp parallel for
    for ( int i = 0; i < Testing::g_repeat; ++i )
    {
        CALL_SUBTEST( ( Testing::run() ) );
    }

    return EXIT_SUCCESS;
}
