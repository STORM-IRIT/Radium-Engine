#ifndef RADIUM_ALGEBRATESTS_HPP_
#define RADIUM_ALGEBRATESTS_HPP_

#include <Core/Math/LinearAlgebra.hpp>
#include <Tests/CoreTests/Tests.hpp>

namespace RaTests {
class AlgebraTests : public Test {
    void run() override {
        // Test the custom vector functions.

        Ra::Core::Math::Vector3 tests[] = {
            {1.34f, 10.34567f, 4.8e5f}, {0.f, -1.42f, -5.3e5f}, {0.9999f, 1.99999f, -1.000001f}};

        for ( uint i = 0; i < 3; ++i )
        {
            const Ra::Core::Math::Vector3& v = tests[i];
            Ra::Core::Math::Vector3 fl( std::floor( v.x() ), std::floor( v.y() ), std::floor( v.z() ) );
            Ra::Core::Math::Vector3 ce( std::ceil( v.x() ), std::ceil( v.y() ), std::ceil( v.z() ) );

            RA_UNIT_TEST( fl == Ra::Core::Math::Vector::floor( v ), "Vector floor function fail" );
            RA_UNIT_TEST( ce == Ra::Core::Math::Vector::ceil( v ), "Vector ceil function fail" );
        }

        // Test the quaternion function.
        Ra::Core::Math::Quaternion qr( 1.f, 2.f, 3.f, 4.f );
        qr.normalize();
        RA_UNIT_TEST( ( 3.14f * qr ).isApprox( qr * 3.14f ),
                      "Quaternion multiplication does not commute" );

        Ra::Core::Math::Quaternion qt, qs;

        Ra::Core::Math::QuaternionUtils::getSwingTwist( qr, qs, qt );

        RA_UNIT_TEST( qr.isApprox( qs * qt ), "Swing decomposition fail." );
        RA_UNIT_TEST( Ra::Core::Math::AngleAxis( qt ).axis().isApprox( Ra::Core::Math::Vector3::UnitZ() ),
                      "Twist should be around z" );
        RA_UNIT_TEST( Ra::Core::Math::AngleAxis( qs ).axis().dot( Ra::Core::Math::Vector3::UnitZ() ) == 0,
                      "Swing should be in xy" );
    }
};
RA_TEST_CLASS( AlgebraTests );
} // namespace RaTests

#endif // RADIUM_ALGEBRATESTS_HPP_
