#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Math/Math.hpp>
#include <Core/Types.hpp>
#include <Tests.hpp>

#include <algorithm>
namespace Ra {
namespace Testing {

void run() {
    // Test the custom vector functions.

    Ra::Core::Vector3 tests[] = {
        {1.34f, 10.34567f, 4.8e5f}, {0.f, -1.42f, -5.3e5f}, {0.9999f, 1.99999f, -1.000001f}};

    for ( uint i = 0; i < 3; ++i )
    {
        const Ra::Core::Vector3& v = tests[i];
        Ra::Core::Vector3 fl( std::floor( v.x() ), std::floor( v.y() ), std::floor( v.z() ) );
        Ra::Core::Vector3 ce( std::ceil( v.x() ), std::ceil( v.y() ), std::ceil( v.z() ) );

        RA_VERIFY( fl == Ra::Core::Math::floor( v ), "Vector floor function fail" );
        RA_VERIFY( ce == Ra::Core::Math::ceil( v ), "Vector ceil function fail" );
    }

    // Test the quaternion function.
    Ra::Core::Quaternion qr( 1.f, 2.f, 3.f, 4.f );
    qr.normalize();
    RA_VERIFY( ( 3.14f * qr ).isApprox( qr * 3.14f ),
               "Quaternion multiplication does not commute" );

    Ra::Core::Quaternion qt, qs;

    Ra::Core::Math::getSwingTwist( qr, qs, qt );

    RA_VERIFY( qr.isApprox( qs * qt ), "Swing decomposition fail." );
    RA_VERIFY( Ra::Core::AngleAxis( qt ).axis().isApprox( Ra::Core::Vector3::UnitZ() ),
               "Twist should be around z" );
    RA_VERIFY( Ra::Core::AngleAxis( qs ).axis().dot( Ra::Core::Vector3::UnitZ() ) == 0,
               "Swing should be in xy" );

    //    using std::clamp;
    //    using Ra::Core::Math::clamp;
    Scalar min = -12.;
    Scalar max = +27.;
    Scalar s = 0.6;

    RA_VERIFY( s == std::clamp( s, min, max ), "Clamp fails on Scalar" );
    RA_VERIFY( min == std::clamp( min, min, max ), "Clamp fails on Scalar" );
    RA_VERIFY( max == std::clamp( max, min, max ), "Clamp fails on Scalar" );
    RA_VERIFY( max == std::clamp( max + s, min, max ), "Clamp fails on Scalar" );
    RA_VERIFY( min == std::clamp( min - s, min, max ), "Clamp fails on Scalar" );

    Ra::Core::Vector3 v{Scalar( 0.1 ), Scalar( 0.2 ), Scalar( 0.3 )};
    Ra::Core::Vector3 v2 = Ra::Core::Math::clamp( v, min, max );
    RA_VERIFY( v2.isApprox( v ), "Clamp fails on Vector and scalar" );

    v = Ra::Core::Math::clamp( Ra::Core::Vector3::Constant( s ), min, max );
    v2 = Ra::Core::Vector3::Constant( s );
    RA_VERIFY( v2.isApprox( v ), "Clamp fails on Vector and scalar" );

    RA_VERIFY( Ra::Core::Vector3::Constant( min ).isApprox(
                   Ra::Core::Math::clamp( Ra::Core::Vector3::Constant( min ), min, max ) ),
               "Clamp fails on Vector and scalar" );

    RA_VERIFY( Ra::Core::Vector3::Constant( max ).isApprox(
                   Ra::Core::Math::clamp( Ra::Core::Vector3::Constant( max ), min, max ) ),
               "Clamp fails on Vector and scalar" );
    RA_VERIFY( Ra::Core::Vector3::Constant( max ).isApprox(
                   Ra::Core::Math::clamp( Ra::Core::Vector3::Constant( max + s ), min, max ) ),
               "Clamp fails on Vector and scalar" );
    RA_VERIFY( Ra::Core::Vector3::Constant( min ).isApprox(
                   Ra::Core::Math::clamp( Ra::Core::Vector3::Constant( min - s ), min, max ) ),
               "Clamp fails on Vector and scalar" );

    RA_VERIFY( Ra::Core::Vector3::Constant( min ).isApprox( Ra::Core::Math::clamp(
                   Ra::Core::Vector3::Constant( min ), Ra::Core::Vector3::Constant( min ),
                   Ra::Core::Vector3::Constant( max ) ) ),
               "Component-wise clamp fails" );

    RA_VERIFY( Ra::Core::Vector3::Constant( max ).isApprox( Ra::Core::Math::clamp(
                   Ra::Core::Vector3::Constant( max ), Ra::Core::Vector3::Constant( min ),
                   Ra::Core::Vector3::Constant( max ) ) ),
               "Component-wise clamp fails" );
    RA_VERIFY( Ra::Core::Vector3::Constant( max ).isApprox( Ra::Core::Math::clamp(
                   Ra::Core::Vector3::Constant( max + s ), Ra::Core::Vector3::Constant( min ),
                   Ra::Core::Vector3::Constant( max ) ) ),
               "Component-wise clamp fails" );
    RA_VERIFY( Ra::Core::Vector3::Constant( min ).isApprox( Ra::Core::Math::clamp(
                   Ra::Core::Vector3::Constant( min - s ), Ra::Core::Vector3::Constant( min ),
                   Ra::Core::Vector3::Constant( max ) ) ),
               "Component-wise clamp fails" );

    RA_VERIFY( Ra::Core::Vector3( min, max, s )
                   .isApprox( Ra::Core::Math::clamp( Ra::Core::Vector3( min - s, max + s, s ),
                                                     Ra::Core::Vector3::Constant( min ),
                                                     Ra::Core::Vector3::Constant( max ) ) ),
               "Component-wise clamp fails" );
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
