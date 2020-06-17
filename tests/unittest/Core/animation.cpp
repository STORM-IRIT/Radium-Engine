#include <Core/Animation/HandleWeightOperation.hpp>
#include <Core/Animation/KeyFramedValue.hpp>
#include <Core/Animation/KeyFramedValueInterpolators.hpp>
#include <catch2/catch.hpp>

using namespace Ra::Core;
using namespace Ra::Core::Animation;

TEST_CASE( "Core/Animation/HandleWeightOperation",
           "[Core][Core/Animation][HandleWeightOperation]" ) {
    static const constexpr int w = 50;
    static const constexpr int h = w;

    WeightMatrix matrix1( w, h );
    matrix1.setIdentity();

    SECTION( "Test normalization" ) {
        // Matrix1 is already normalized
        REQUIRE( !normalizeWeights( matrix1 ) );

        matrix1.coeffRef( w / 3, h / 2 ) = 0.8_ra;

        // Matrix1 normalization ok
        REQUIRE( normalizeWeights( matrix1 ) );

        WeightMatrix matrix2 = matrix1;
        matrix2 *= 0.5_ra;

        WeightMatrix matrix3 = partitionOfUnity( matrix2 );

        // Matrix2 needs to be normalized
        REQUIRE( normalizeWeights( matrix2 ) );
        // Matrix3 is already normalized
        REQUIRE( !normalizeWeights( matrix3 ) );
        // Two matrices are equivalent after normalization
        REQUIRE( matrix1.isApprox( matrix2 ) );
        // Two matrices are equivalent after partition of unity
        REQUIRE( matrix1.isApprox( matrix3 ) );

        matrix2.coeffRef( w / 3, h / 2 ) = std::nanf( "" );
        // Should not find NaN in this matrix
        REQUIRE( checkWeightMatrix( matrix1, false ) );
        // Should find NaN in this matrix
        REQUIRE( !checkWeightMatrix( matrix2, false ) );
    }
}

TEST_CASE( "Core/Animation/KeyFramedValue", "[Core][Core/Animation][KeyFramedValue]" ) {

    KeyFramedValue<Scalar> kf( 2_ra, 2_ra );

    auto checkValues = []( auto& p, Scalar time, Scalar value ) {
        REQUIRE( Math::areApproxEqual( p.first, time ) );
        REQUIRE( Math::areApproxEqual( p.second, value ) );
    };

    auto checkSorting = []( auto& kf ) {
        for ( size_t i = 1; i < kf.size(); ++i )
        {
            REQUIRE( kf[i - 1].first < kf[i].first );
        }
    };

    SECTION( "Test keyframe manipulation" ) {
        // There is one keyframe
        REQUIRE( kf.size() == 1 );
        // We cannot remove it
        REQUIRE( !kf.removeKeyFrame( 0 ) );
        // It should be (2,2)
        checkValues( kf[0], 2_ra, 2_ra );

        // adding before first
        kf.insertKeyFrame( 0_ra, 0_ra );
        // There should be 2 keyframes
        REQUIRE( kf.size() == 2 );
        // These should be sorted
        checkSorting( kf );
        // The first one should be (0,0)
        checkValues( kf[0], 0_ra, 0_ra );
        // The second one should be (2,2)
        checkValues( kf[1], 2_ra, 2_ra );

        // adding in between
        kf.insertKeyFrame( 1_ra, 1_ra );
        // There should be 3 keyframes
        REQUIRE( kf.size() == 3 );
        // These should be sorted
        checkSorting( kf );
        // The first one should be (0,0)
        checkValues( kf[0], 0_ra, 0_ra );
        // The second one should be (1,1)
        checkValues( kf[1], 1_ra, 1_ra );
        // The third one should be (2,2)
        checkValues( kf[2], 2_ra, 2_ra );

        // adding after last
        kf.insertKeyFrame( 3_ra, 3_ra );
        // There should still be 4 keyframes
        REQUIRE( kf.size() == 4 );
        // These should be sorted
        checkSorting( kf );
        // The first one should be (0,0)
        checkValues( kf[0], 0_ra, 0_ra );
        // The second one should be (1,1)
        checkValues( kf[1], 1_ra, 1_ra );
        // The third one should be (2,2)
        checkValues( kf[2], 2_ra, 2_ra );
        // The fourth one should be (3,3)
        checkValues( kf[3], 3_ra, 3_ra );

        {
            auto kf2 = kf;
            // There should be 4 keyframes
            REQUIRE( kf2.size() == 4 );

            // Should be able to remove inside
            REQUIRE( kf2.removeKeyFrame( 2 ) );
            // There should be 3 keyframes
            REQUIRE( kf2.size() == 3 );
            // These should be sorted
            checkSorting( kf2 );
            // The first one should be (0,0)
            checkValues( kf2[0], 0_ra, 0_ra );
            // The second one should be (1,1)
            checkValues( kf2[1], 1_ra, 1_ra );
            // The third one should be (3,3)
            checkValues( kf2[2], 3_ra, 3_ra );

            // Should be able to remove last
            REQUIRE( kf2.removeKeyFrame( 2 ) );
            // There should be 2 keyframes
            REQUIRE( kf2.size() == 2 );
            // These should be sorted
            checkSorting( kf2 );
            // The first one should be (0,0)
            checkValues( kf2[0], 0_ra, 0_ra );
            // The second one should be (1,1)
            checkValues( kf2[1], 1_ra, 1_ra );

            // Should be able to remove first
            REQUIRE( kf2.removeKeyFrame( 0 ) );
            // There should be 1 keyframe
            REQUIRE( kf2.size() == 1 );
            // It should be (1,1)
            checkValues( kf2[0], 1_ra, 1_ra );

            // We cannot remove it
            REQUIRE( !kf2.removeKeyFrame( 0 ) );
        }

        // Replacing value
        kf.insertKeyFrame( 3_ra, 2_ra );
        // There should still be 4 keyframes
        REQUIRE( kf.size() == 4 );
        // These should be sorted
        checkSorting( kf );
        // The first one should be (0,0)
        checkValues( kf[0], 0_ra, 0_ra );
        // The second one should be (1,1)
        checkValues( kf[1], 1_ra, 1_ra );
        // The third one should be (2,2)
        checkValues( kf[2], 2_ra, 2_ra );
        // The fourth one should be (3,2)
        checkValues( kf[3], 3_ra, 2_ra );

        // Moving keyframe to the front
        kf.moveKeyFrame( 1, -1_ra );
        // There should still be 4 keyframes
        REQUIRE( kf.size() == 4 );
        // These should be sorted
        checkSorting( kf );
        // The first one should be (-1,1)
        checkValues( kf[0], -1_ra, 1_ra );
        // The second one should be (0,0)
        checkValues( kf[1], 0_ra, 0_ra );
        // The third one should be (2,2)
        checkValues( kf[2], 2_ra, 2_ra );
        // The fourth one should be (3,2)
        checkValues( kf[3], 3_ra, 2_ra );

        // Moving keyframe in between
        kf.moveKeyFrame( 1, 2.5_ra );
        // There should still be 4 keyframes
        REQUIRE( kf.size() == 4 );
        // These should be sorted
        checkSorting( kf );
        // The first one should be (-1,1)
        checkValues( kf[0], -1_ra, 1_ra );
        // The second one should be (2,2)
        checkValues( kf[1], 2_ra, 2_ra );
        // The third one should be (2.5,0)
        checkValues( kf[2], 2.5_ra, 0_ra );
        // The fourth one should be (3,2)
        checkValues( kf[3], 3_ra, 2_ra );

        // Moving keyframe to the end
        kf.moveKeyFrame( 1, 4_ra );
        // There should still be 4 keyframes
        REQUIRE( kf.size() == 4 );
        // These should be sorted
        checkSorting( kf );
        // The first one should be (-1,1)
        checkValues( kf[0], -1_ra, 1_ra );
        // The second one should be (2.5,0)
        checkValues( kf[1], 2.5_ra, 0_ra );
        // The third one should be (3,2)
        checkValues( kf[2], 3_ra, 2_ra );
        // The fourth one should be (4,2)
        checkValues( kf[3], 4_ra, 2_ra );

        // Evaluating before first time should give first value
        REQUIRE( Math::areApproxEqual( kf.at( kf[0].first - 1, linearInterpolate<Scalar> ),
                                       kf[0].second ) );
        // Evaluating at keyframe time should give keyframe value
        for ( size_t i = 0; i < kf.size(); ++i )
        {
            REQUIRE( Math::areApproxEqual( kf.at( kf[i].first, linearInterpolate<Scalar> ),
                                           kf[i].second ) );
        }
        // Evaluating after last time should give last value
        REQUIRE(
            Math::areApproxEqual( kf.at( kf[kf.size() - 1].first + 1, linearInterpolate<Scalar> ),
                                  kf[kf.size() - 1].second ) );
        // Check some in-between interpolations
        REQUIRE( Math::areApproxEqual( kf.at( 0_ra, linearInterpolate<Scalar> ), 5_ra / 7 ) );
        REQUIRE( Math::areApproxEqual( kf.at( 2.75_ra, linearInterpolate<Scalar> ), 1_ra ) );
        REQUIRE( Math::areApproxEqual( kf.at( 3.7_ra, linearInterpolate<Scalar> ), 2_ra ) );

        // Insert before first using interpolation
        kf.insertInterpolatedKeyFrame( -2_ra, linearInterpolate<Scalar> );
        // There should still be 5 keyframes
        REQUIRE( kf.size() == 5 );
        // These should be sorted
        checkSorting( kf );
        // The first one should be (-2,1)
        checkValues( kf[0], -2_ra, 1_ra );
        // The second one should be (-1,1)
        checkValues( kf[1], -1_ra, 1_ra );
        // The third one should be (2.5,0)
        checkValues( kf[2], 2.5_ra, 0_ra );
        // The fourth one should be (3,2)
        checkValues( kf[3], 3_ra, 2_ra );
        // The fifth one should be (4,2)
        checkValues( kf[4], 4_ra, 2_ra );

        // Insert in-between using interpolation
        kf.insertInterpolatedKeyFrame( 2.75_ra, linearInterpolate<Scalar> );
        // There should still be 6 keyframes
        REQUIRE( kf.size() == 6 );
        // These should be sorted
        checkSorting( kf );
        // The first one should be (-2,1)
        checkValues( kf[0], -2_ra, 1_ra );
        // The second one should be (-1,1)
        checkValues( kf[1], -1_ra, 1_ra );
        // The third one should be (2.5,0)
        checkValues( kf[2], 2.5_ra, 0_ra );
        // The fourth one should be (2.75,1)
        checkValues( kf[3], 2.75_ra, 1_ra );
        // The fifth one should be (3,2)
        checkValues( kf[4], 3_ra, 2_ra );
        // The sixth one should be (4,2)
        checkValues( kf[5], 4_ra, 2_ra );

        // Insert after last using interpolation
        kf.insertInterpolatedKeyFrame( 5_ra, linearInterpolate<Scalar> );
        // There should still be 7 keyframes
        REQUIRE( kf.size() == 7 );
        // These should be sorted
        checkSorting( kf );
        // The first one should be (-2,1)
        checkValues( kf[0], -2_ra, 1_ra );
        // The second one should be (-1,1)
        checkValues( kf[1], -1_ra, 1_ra );
        // The third one should be (2.5,0)
        checkValues( kf[2], 2.5_ra, 0_ra );
        // The fourth one should be (2.75,1)
        checkValues( kf[3], 2.75_ra, 1_ra );
        // The fifth one should be (3,2)
        checkValues( kf[4], 3_ra, 2_ra );
        // The sixth one should be (4,2)
        checkValues( kf[5], 4_ra, 2_ra );
        // The seventh one should be (5,2)
        checkValues( kf[6], 5_ra, 2_ra );
    }
}
