#include <Core/Geometry/Distance/DistanceQueries.hpp>
#include <Tests.hpp>

namespace Ra {
namespace Testing {
void run() {
    using Ra::Core::Vector3;
    Vector3 a( -2.f, 0.f, 0.f );
    Vector3 b( 2.f, 0.f, 0.f );
    Vector3 c( 0.f, 3.f, 0.f );
    Vector3 d( 0.f, -3.f, 0.f );
    Vector3 e( -4.f, -3.f, 0.f );
    Vector3 f( 4.f, 3.f, 0.f );

    // Test point to triangle query
    Scalar distPointToTri =
        std::sqrt( Ra::Core::DistanceQueries::pointToTriSq( c, a, b, d ).distanceSquared );
    RA_VERIFY( distPointToTri == ( c - ( 0.5 * ( a + b ) ) ).norm(),

                  "Distance point to triangle not ok." );

    // Test line to segment distance query
    const Vector3& lineOrigin = a;
    Vector3 lineDirection = d - a;
    const Vector3& segCenter = (Scalar)0.5 * ( c + b );
    Vector3 segDirection = b - c;
    Scalar segExtent = (Scalar)0.5 * std::sqrt( ( b - c ).dot( b - c ) );
    Scalar distLineToSeg = Ra::Core::DistanceQueries::lineToSegSq(
                               lineOrigin, lineDirection, segCenter, segDirection, segExtent )
                               .sqrDistance;
    RA_VERIFY( distLineToSeg == Ra::Core::DistanceQueries::pointToSegmentSq( a, c, b - c ),

                  "Distance line to segment not ok." );


    // Test line to triangle distance query
    Vector3 v[3] = {a, d, e};
    Scalar distLineToTri =
        Ra::Core::DistanceQueries::lineToTriSq( segCenter, segDirection, v ).sqrDistance;
    RA_VERIFY( distLineToTri == Ra::Core::DistanceQueries::pointToSegmentSq( a, c, b - c ),

                  "Distance line to triangle not ok." );


    // Test segment to triangle distance query
    Scalar distSegToTri =
        Ra::Core::DistanceQueries::segmentToTriSq( segCenter, segDirection, segExtent, v )
            .sqrDistance;
    RA_VERIFY( distSegToTri == Ra::Core::DistanceQueries::pointToSegmentSq( a, c, b - c ),

                  "Distance segment to triangle not ok." );

    // Test triangle to triangle distance query
    Vector3 v2[3] = {c, f, b};
    Scalar distTriToTri = Ra::Core::DistanceQueries::triangleToTriSq( v, v2 ).sqrDistance;
    RA_VERIFY( distTriToTri == Ra::Core::DistanceQueries::pointToSegmentSq( a, c, b - c ),
                  "Distance triangle to triangle not ok." );
}
} // namespace Testing
} // namespace Ra

int main(int argc, const char **argv) {
    using namespace Ra;

    if(!Testing::init_testing(1, argv))

    {
        return EXIT_FAILURE;
    }

#pragma omp parallel for
   for(int i = 0; i < Testing::g_repeat; ++i)
    {
        CALL_SUBTEST(( Testing::run() ));

    }

    return EXIT_SUCCESS;
}
