#include <Core/Animation/Handle/HandleWeightOperation.hpp>
#include <Tests.hpp>

namespace Ra {
namespace Testing {
// tests :
//  - normalizeWeights
//  - partitionOfUnity
//  - check_NAN
//
// \todo Add other functions
void run() {
    static const constexpr int w = 50;
    static const constexpr int h = w;
    using Ra::Core::Animation::WeightMatrix;

    WeightMatrix matrix1( w, h );
    matrix1.setIdentity();


    RA_VERIFY( !Ra::Core::Animation::normalizeWeights( matrix1 ),
                  "Matrix1 is already normalized" );

    matrix1.coeffRef( w / 3, h / 2 ) = 0.8;

    RA_VERIFY( Ra::Core::Animation::normalizeWeights( matrix1 ),
                  "Matrix1 normalization ok" );

    WeightMatrix matrix2 = matrix1;
    matrix2 *= 0.5;

    WeightMatrix matrix3 = Ra::Core::Animation::partitionOfUnity( matrix2 );


    RA_VERIFY( Ra::Core::Animation::normalizeWeights( matrix2 ),
                  "Matrix2 needs to be normalized" );

    RA_VERIFY( !Ra::Core::Animation::normalizeWeights( matrix3 ),
                  "Matrix3 is already normalized" );

    RA_VERIFY( matrix1.isApprox( matrix2 ),
                  "Two matrices are equivalent after normalization" );

    RA_VERIFY( matrix1.isApprox( matrix3 ),
                  "Two matrices are equivalent after partition of unity" );

    matrix2.coeffRef( w / 3, h / 2 ) = std::nan( "" );
    RA_VERIFY( Ra::Core::Animation::checkWeightMatrix( matrix1, false ),
                  "Should not find NaN in this matrix" );
    RA_VERIFY( !Ra::Core::Animation::checkWeightMatrix( matrix2, false ),
                  "Should find NaN in this matrix" );
}

} // namespace Testing
} // namespace Ra


int main(int argc, const char **argv) {
    using namespace Ra;

    if(!Testing::init_testing(1, argv))
    {
        return EXIT_FAILURE;
    }

#pragma omp parallel for
   for(int i = 0; i < Testing::g_repeat; ++i)
    {
        CALL_SUBTEST(( Testing::run() ));

    }

    return EXIT_SUCCESS;
}
