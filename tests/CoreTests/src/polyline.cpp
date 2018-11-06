#include <Core/Math/PolyLine.hpp>
#include <Tests.hpp>

namespace Ra {
namespace Testing {

void run() {
    // 2 points polyline
    {
        Ra::Core::Vector3Array v2{{1, 2, 3}, {4, 5, 6}};
        Ra::Core::PolyLine p( v2 );

        Ra::Core::Vector3 m = 0.5f * ( v2[0] + v2[1] );

        RA_VERIFY( p.f( 0 ) == v2[0], "Parametrization fail" );
        RA_VERIFY( p.f( 1 ) == v2[1], "Parametrization fail" );
        RA_VERIFY( p.f( 0.5f ).isApprox( m ), "Parametrization fail" );
        RA_VERIFY( Ra::Core::Math::areApproxEqual( p.distance( m ), Scalar( 0. ) ),
                   "Distance fail" );
    }
    // 4 points polyline
    {

        Ra::Core::Vector3Array v4{{2, 3, 5}, {7, 11, 13}, {17, 23, 29}, {-1, -1, 30}};

        Ra::Core::PolyLine p( v4 );

        RA_VERIFY( p.f( 0 ) == v4[0], "Parametrization fail" );
        RA_VERIFY( p.f( 1 ) == v4[3], "Parametrization fail" );
        RA_VERIFY( p.f( -10. ) == p.f( 0 ), "Parametrization clamp fail" );
        RA_VERIFY( p.f( 10. ) == p.f( 1 ), "Parametrization clamp fail" );

        for ( const auto& x : v4 )
        {
            RA_VERIFY( p.distance( x ) == Scalar( 0. ), "Distance fail" );
        }
    }
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
