#include <Core/Animation/HandleWeightOperation.hpp>
#include <catch2/catch.hpp>

TEST_CASE( "Core/Animation/HandleWeightOperation",
           "[Core][Core/Animation][HandleWeightOperation]" ) {
    using namespace Ra::Core;
    static const constexpr int w = 50;
    static const constexpr int h = w;
    using Ra::Core::Animation::WeightMatrix;

    WeightMatrix matrix1( w, h );
    matrix1.setIdentity();

    SECTION( "Test normalization" ) {
        // Matrix1 is already normalized
        REQUIRE( !Ra::Core::Animation::normalizeWeights( matrix1 ) );

        matrix1.coeffRef( w / 3, h / 2 ) = 0.8_ra;

        // Matrix1 normalization ok
        REQUIRE( Ra::Core::Animation::normalizeWeights( matrix1 ) );

        WeightMatrix matrix2 = matrix1;
        matrix2 *= 0.5_ra;

        WeightMatrix matrix3 = Ra::Core::Animation::partitionOfUnity( matrix2 );

        // Matrix2 needs to be normalized
        REQUIRE( Ra::Core::Animation::normalizeWeights( matrix2 ) );
        // Matrix3 is already normalized
        REQUIRE( !Ra::Core::Animation::normalizeWeights( matrix3 ) );
        // Two matrices are equivalent after normalization
        REQUIRE( matrix1.isApprox( matrix2 ) );
        // Two matrices are equivalent after partition of unity
        REQUIRE( matrix1.isApprox( matrix3 ) );

        matrix2.coeffRef( w / 3, h / 2 ) = std::nanf( "" );
        // Should not find NaN in this matrix
        REQUIRE( Ra::Core::Animation::checkWeightMatrix( matrix1, false ) );
        // Should find NaN in this matrix
        REQUIRE( !Ra::Core::Animation::checkWeightMatrix( matrix2, false ) );
    }
}
