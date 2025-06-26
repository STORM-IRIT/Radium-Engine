#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Math/Math.hpp>
#include <Core/Types.hpp>

#include <algorithm> // clamp
#include <catch2/catch_test_macros.hpp>
#include <cmath>

TEST_CASE( "Core/Math/Algebra", "[unittests][Core][Core/Math][Algebra]" ) {
    using namespace Ra::Core;
    SECTION( "Test custom vector functions" ) {

        Vector3 tests[] = { { 1.34f, 10.34567f, 4.8e5f },
                            { 0.f, -1.42f, -5.3e5f },
                            { 0.9999f, 1.99999f, -1.000001f } };

        for ( uint i = 0; i < 3; ++i ) {
            const Vector3& v = tests[i];
            Vector3 fl( std::floor( v.x() ), std::floor( v.y() ), std::floor( v.z() ) );
            Vector3 ce( std::ceil( v.x() ), std::ceil( v.y() ), std::ceil( v.z() ) );

            REQUIRE( fl == Math::floor( v ) );
            REQUIRE( ce == Math::ceil( v ) );
        }
    }

    SECTION( "Test quaternion functions" ) {
        Quaternion qr( 1.f, 2.f, 3.f, 4.f );
        qr.normalize();
        // Quaternion multiplication does not commute
        REQUIRE( ( 3.14f * qr ).isApprox( qr * 3.14f ) );

        Quaternion qt, qs;

        Math::getSwingTwist( qr, qs, qt );

        // Swing decomposition
        REQUIRE( qr.isApprox( qs * qt ) );
        // Twist should be around z
        REQUIRE( AngleAxis( qt ).axis().isApprox( Vector3::UnitZ() ) );
        // Swing should be in xy
        REQUIRE( Math::areApproxEqual( AngleAxis( qs ).axis().dot( Vector3::UnitZ() ), 0_ra ) );
    }

    SECTION( "Test clamp" ) {
        //    using std::clamp;
        //    using Math::clamp;
        Scalar min = -12_ra;
        Scalar max = +27_ra;
        Scalar s   = 0.6_ra;

        REQUIRE( Math::areApproxEqual( s, std::clamp( s, min, max ) ) );
        REQUIRE( Math::areApproxEqual( min, std::clamp( min, min, max ) ) );
        REQUIRE( Math::areApproxEqual( max, std::clamp( max, min, max ) ) );
        REQUIRE( Math::areApproxEqual( max, std::clamp( max + s, min, max ) ) );
        REQUIRE( Math::areApproxEqual( min, std::clamp( min - s, min, max ) ) );

        Vector3 v { Scalar( 0.1 ), Scalar( 0.2 ), Scalar( 0.3 ) };
        Vector3 v2 = Math::clamp( v, min, max );
        REQUIRE( v2.isApprox( v ) );

        v  = Math::clamp( Vector3::Constant( s ), min, max );
        v2 = Vector3::Constant( s );
        REQUIRE( v2.isApprox( v ) );

        REQUIRE( Vector3::Constant( min ).isApprox(
            Math::clamp( Vector3::Constant( min ), min, max ) ) );

        REQUIRE( Vector3::Constant( max ).isApprox(
            Math::clamp( Vector3::Constant( max ), min, max ) ) );
        REQUIRE( Vector3::Constant( max ).isApprox(
            Math::clamp( Vector3::Constant( max + s ), min, max ) ) );
        REQUIRE( Vector3::Constant( min ).isApprox(
            Math::clamp( Vector3::Constant( min - s ), min, max ) ) );

        REQUIRE( Vector3::Constant( min ).isApprox( Math::clamp(
            Vector3::Constant( min ), Vector3::Constant( min ), Vector3::Constant( max ) ) ) );

        REQUIRE( Vector3::Constant( max ).isApprox( Math::clamp(
            Vector3::Constant( max ), Vector3::Constant( min ), Vector3::Constant( max ) ) ) );
        REQUIRE( Vector3::Constant( max ).isApprox( Math::clamp(
            Vector3::Constant( max + s ), Vector3::Constant( min ), Vector3::Constant( max ) ) ) );
        REQUIRE( Vector3::Constant( min ).isApprox( Math::clamp(
            Vector3::Constant( min - s ), Vector3::Constant( min ), Vector3::Constant( max ) ) ) );

        REQUIRE( Vector3( min, max, s )
                     .isApprox( Math::clamp( Vector3( min - s, max + s, s ),
                                             Vector3::Constant( min ),
                                             Vector3::Constant( max ) ) ) );
    }

    SECTION( "smootsteop" ) {
        using Math::smoothstep;
        Scalar min = -12_ra;
        Scalar max = +27_ra;
        Scalar s   = 0.6_ra;

        REQUIRE( Math::areApproxEqual( 0_ra, smoothstep( min, max, min ) ) );
        REQUIRE( Math::areApproxEqual( 1_ra, smoothstep( min, max, max ) ) );
        REQUIRE( Math::areApproxEqual( 1_ra, smoothstep( min, max, max + s ) ) );
        REQUIRE( Math::areApproxEqual( 0_ra, smoothstep( min, max, min - s ) ) );

        REQUIRE( Math::areApproxEqual( 0.5f, smoothstep( -1.f, 1.f, 0.f ) ) );
        REQUIRE( Math::areApproxEqual( 1.f, smoothstep( 0.f, 0.f, 1.f ) ) );
        REQUIRE( Math::areApproxEqual( 0.f, smoothstep( 1.f, -1.f, 1.f ) ) );

        REQUIRE( Math::areApproxEqual( 0.5f, smoothstep( -1.0f, 1.0f, 0.0f ) ) );
        REQUIRE( Math::areApproxEqual( 1.0f, smoothstep( 0.0f, 0.0f, 1.0f ) ) );
        REQUIRE( Math::areApproxEqual( 0.0f, smoothstep( 1.0f, -1.0f, 1.0f ) ) );

        Vector3 v { 0_ra, 0_ra, 0_ra };
        auto v2 = smoothstep( -1_ra, 1_ra, v );
        REQUIRE( v2.isApprox( Vector3 { 0.5_ra, 0.5_ra, 0.5_ra } ) );

        Matrix2 m;
        m << 0_ra, 0_ra, 0_ra, 0_ra;
        Matrix2 m2;
        m2 << 0.5_ra, 0.5_ra, 0.5_ra, 0.5_ra;
        auto m3 = smoothstep( -1_ra, 1_ra, m );
        REQUIRE( m3.isApprox( m2 ) );
    }
}
