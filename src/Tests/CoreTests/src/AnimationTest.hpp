#ifndef RADIUM_ANIMATIONTESTS_HPP_
#define RADIUM_ANIMATIONTESTS_HPP_

#include <Core/Animation/Handle/HandleWeightOperation.hpp>
#include <Tests/Tests.hpp>

using Ra::Core::Animation::WeightMatrix;

namespace RaTests {
class HandleWeightTests : public Test {
    // tests :
    //  - normalizeWeights
    //  - partitionOfUnity
    //  - check_NAN
    //
    // \todo Add other functions
    void run() override {
        static const constexpr int w = 50;
        static const constexpr int h = w;

        WeightMatrix matrix1( w, h );
        matrix1.setIdentity();

        RA_UNIT_TEST( !Ra::Core::Animation::normalizeWeights( matrix1 ),
                      "Matrix1 is already normalized" );

        matrix1.coeffRef( w / 3, h / 2 ) = 0.8;

        RA_UNIT_TEST( Ra::Core::Animation::normalizeWeights( matrix1 ),
                      "Matrix1 normalization ok" );

        WeightMatrix matrix2 = matrix1;
        matrix2 *= 0.5;

        WeightMatrix matrix3 = Ra::Core::Animation::partitionOfUnity( matrix2 );

        RA_UNIT_TEST( Ra::Core::Animation::normalizeWeights( matrix2 ),
                      "Matrix2 needs to be normalized" );

        RA_UNIT_TEST( !Ra::Core::Animation::normalizeWeights( matrix3 ),
                      "Matrix3 is already normalized" );

        RA_UNIT_TEST( matrix1.isApprox( matrix2 ),
                      "Two matrices are equivalent after normalization" );

        RA_UNIT_TEST( matrix1.isApprox( matrix3 ),
                      "Two matrices are equivalent after partition of unity" );

        matrix2.coeffRef( w / 3, h / 2 ) = std::nan( "" );
        RA_UNIT_TEST( Ra::Core::Animation::checkWeightMatrix( matrix1, false ),
                      "Should not find NaN in this matrix" );
        RA_UNIT_TEST( !Ra::Core::Animation::checkWeightMatrix( matrix2, false ),
                      "Should find NaN in this matrix" );
    }
};

RA_TEST_CLASS( HandleWeightTests )
} // namespace RaTests

#endif // RADIUM_GEOMETRYTESTS_HPP_
