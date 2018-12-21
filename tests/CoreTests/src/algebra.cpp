#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Math/Types.hpp>
#include <Tests.hpp>

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

        RA_VERIFY( fl == Ra::Core::Vector::floor( v ), "Vector floor function fail" );
        RA_VERIFY( ce == Ra::Core::Vector::ceil( v ), "Vector ceil function fail" );
    }

    // Test the quaternion function.
    Ra::Core::Quaternion qr( 1.f, 2.f, 3.f, 4.f );
    qr.normalize();
    RA_VERIFY( ( 3.14f * qr ).isApprox( qr * 3.14f ),
               "Quaternion multiplication does not commute" );

    Ra::Core::Quaternion qt, qs;

    Ra::Core::QuaternionUtils::getSwingTwist( qr, qs, qt );

    RA_VERIFY( qr.isApprox( qs * qt ), "Swing decomposition fail." );
    RA_VERIFY( Ra::Core::AngleAxis( qt ).axis().isApprox( Ra::Core::Vector3::UnitZ() ),
               "Twist should be around z" );
    RA_VERIFY( Ra::Core::AngleAxis( qs ).axis().dot( Ra::Core::Vector3::UnitZ() ) == 0,
               "Swing should be in xy" );
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
