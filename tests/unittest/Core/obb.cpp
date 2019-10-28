#include <Core/Geometry/Obb.hpp>
#include <Core/Math/Math.hpp>
#include <Core/Types.hpp>
#include <catch2/catch.hpp>

TEST_CASE( "Core/Geometry/Obb", "[Core][Core/Geometry][Obb]" ) {

    using namespace Ra::Core;
    SECTION( "Simple tests" ) {
        // Check Obb and empty aabb
        Aabb aabb;
        Geometry::Obb obb( aabb, Transform::Identity() );
        REQUIRE( obb.toAabb().isEmpty() );
        REQUIRE( obb.toAabb().isApprox( aabb ) );
        obb.transform().rotate( AngleAxis( Math::Pi, Vector3::UnitX() ) );
        REQUIRE( obb.toAabb().isEmpty() );
        REQUIRE( obb.toAabb().isApprox( aabb ) );
    }
}
