#ifndef RADIUM_RAYCAST_TESTS_HPP_
#define RADIUM_RAYCAST_TESTS_HPP_

#include <Core/Math/RayCast.hpp>
#include <Tests/Tests.hpp>

namespace RaTests {

class RayCastAabbTests : public Test {
    void run() override {
        Ra::Core::Aabb ones( -Ra::Core::Vector3::Ones(), Ra::Core::Vector3::Ones() );
        // For all directions x, y, z
        for ( int i = 0; i < 3; ++i )
        {
            // From negative x and positive x
            for ( int sig = -1; sig < 2; sig += 2 )
            {
                Ra::Core::Vector3 s = 3 * sig * Ra::Core::Vector3::Unit( i );
                Ra::Core::Vector3 d = -sig * Ra::Core::Vector3::Unit( i );

                for ( int p = -10; p < 11; p++ )
                {
                    for ( int q = -10; q < 11; q++ )
                    {
                        Ra::Core::Vector3 dir =
                            d + float( p ) / 10.f * Ra::Core::Vector3::Unit( ( i + 1 ) % 3 ) +
                            float( q ) / 10.f * Ra::Core::Vector3::Unit( ( i + 2 ) % 3 );

                        // Fire a ray towards the box (some hit, some miss).
                        {
                            Ra::Core::Ray r = Ra::Core::Ray( s, dir.normalized() );

                            Scalar t = 0.f;
                            Ra::Core::Vector3 n = Ra::Core::Vector3::Zero();
                            const bool result = Ra::Core::RayCast::vsAabb( r, ones, t, n );

                            if ( std::abs( p ) <= 5 && std::abs( q ) <= 5 )
                            {
                                RA_UNIT_TEST( result, "The ray should have hit" );
                                RA_UNIT_TEST( n.dot( sig * Ra::Core::Vector3::Unit( i ) ) == 1.f,
                                              "Wrong normal" );
                                RA_UNIT_TEST(
                                    Ra::Core::Math::areApproxEqual( r.pointAt( t )[i], sig ),
                                    "Wrong hit point" );
                            }
                            else
                            { RA_UNIT_TEST( !result, "The ray should have missed" ); }
                        }

                        // Fire a ray on the other direction (which should miss)
                        {
                            Ra::Core::Ray r = Ra::Core::Ray( s, -dir.normalized() );

                            Scalar t;
                            Ra::Core::Vector3 n;
                            const bool result = Ra::Core::RayCast::vsAabb( r, ones, t, n );

                            RA_UNIT_TEST( !result, "The ray should have missed (t<0)" );
                        }

                        // Fire a ray from within the box.
                        {
                            Ra::Core::Ray r =
                                Ra::Core::Ray( Ra::Core::Vector3::Zero(), dir.normalized() );

                            Scalar t;
                            Ra::Core::Vector3 n{0, 0, 0};
                            const bool result = Ra::Core::RayCast::vsAabb( r, ones, t, n );

                            RA_UNIT_TEST( result, "The ray should have hit (inside hit)" );
                            RA_UNIT_TEST( t == 0, "Hit should be at origin" );
                            RA_UNIT_TEST(
                                Ra::Core::Math::areApproxEqual( n.dot( dir.normalized() ), -1.f ),
                                "Wrong normal (inside hit)" );
                        }
                    }
                }
            }
        }
    }
};
RA_TEST_CLASS( RayCastAabbTests );
} // namespace RaTests

#endif // RADIUM_RAYCAST_TESTS_HPP_
