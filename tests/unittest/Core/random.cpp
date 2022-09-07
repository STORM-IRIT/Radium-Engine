#include <catch2/catch.hpp>
#include <unittestUtils.hpp>

#include <iomanip>
#include <iostream>

#include <Core/Random/RandomPointSet.hpp>
using namespace Ra::Core::Random;

TEST_CASE( "Core/Random/RandomPointSet", "[Core][Core/Random][PointSet]" ) {
    SECTION( "Fibonacci sequence" ) {
        std::array<Scalar, 5> fib_verif { 0_ra,
                                          0.61803400516510009765625_ra,
                                          1.2360680103302001953125_ra,
                                          1.8541018962860107421875_ra,
                                          2.472136020660400390625_ra };
        FibonacciSequence fib { 2 };
        // Our fibonacci sequence is only defined for more than 5 points
        REQUIRE( fib.range() == 5 );
        for ( size_t i = 0; i < 5; ++i ) {
            REQUIRE( isApprox( fib( i ), fib_verif[i] ) );
        }
    }

    SECTION( "VanDerCorput sequence" ) {
        std::array<Scalar, 5> vdc_verif { 0_ra, 0.5_ra, 0.25_ra, 0.75_ra, 0.125_ra };
        VanDerCorputSequence vdc;
        for ( size_t i = 0; i < 5; ++i ) {
            REQUIRE( isApprox( vdc( i ), vdc_verif[i] ) );
        }
    }

    SECTION( "Fibonacci point set" ) {
        std::array<std::pair<Scalar, Scalar>, 5> fibseq_verif {
            std::pair<Scalar, Scalar> { 0_ra, 0_ra / 5_ra },
            { 0.61803400516510009765625_ra, 1_ra / 5_ra },
            { 1.2360680103302001953125_ra, 2_ra / 5_ra },
            { 1.8541018962860107421875_ra, 3_ra / 5_ra },
            { 2.472136020660400390625_ra, 4_ra / 5_ra } };
        FibonacciPointSet fibs { 5 };
        for ( size_t i = 0; i < 5; ++i ) {
            auto v = fibs( i );
            REQUIRE( isApprox( v[0], fibseq_verif[i].first ) );
            REQUIRE( isApprox( v[1], fibseq_verif[i].second ) );
        }
    }

    SECTION( "Hammersley point set" ) {
        std::array<std::pair<Scalar, Scalar>, 5> seq_verif {
            std::pair<Scalar, Scalar> { 0_ra, 0_ra / 5_ra },
            { 1_ra / 5_ra, 0.5_ra },
            { 2_ra / 5_ra, 0.25_ra },
            { 3_ra / 5_ra, 0.75_ra },
            { 4_ra / 5_ra, 0.125_ra } };
        HammersleyPointSet seq { 5 };
        for ( size_t i = 0; i < 5; ++i ) {
            auto v = seq( i );
            REQUIRE( isApprox( v[0], seq_verif[i].first ) );
            REQUIRE( isApprox( v[1], seq_verif[i].second ) );
        }
    }

    // todo, verify if the sequence is always the same (it should) on any systems/run
    SECTION( "MersenneTwister point set" ) {
        std::array<std::pair<Scalar, Scalar>, 5> seq_verif {
            std::pair<Scalar, Scalar> { 0.59284460544586181640625_ra,
                                        0.844265758991241455078125_ra },
            { 0.857945621013641357421875_ra, 0.847251713275909423828125_ra },
            { 0.623563706874847412109375_ra, 0.384381711483001708984375_ra },
            { 0.2975346148014068603515625_ra, 0.056712977588176727294921875_ra },
            { 0.2726562917232513427734375_ra, 0.477665126323699951171875_ra } };
        MersenneTwisterPointSet seq { 5 };
        std::cout << std::setprecision( 32 );
        for ( size_t i = 0; i < 5; ++i ) {
            auto v = seq( i );
            REQUIRE( isApprox( v[0], seq_verif[i].first ) );
            REQUIRE( isApprox( v[1], seq_verif[i].second ) );
        }
    }

    SECTION( "SphericalPointSet point set (Hammersley)" ) {
        std::array<std::pair<Scalar, Scalar>, 5> seq_verif {
            std::pair<Scalar, Scalar> { 1.5099580252808664226904511451721e-07_ra, 0_ra },
            { 0.3090169727802276611328125_ra, 0.951056540012359619140625_ra },
            { -0.700629293918609619140625_ra, 0.50903689861297607421875_ra },
            { -0.700629055500030517578125_ra, -0.509037196636199951171875_ra },
            { 0.204395592212677001953125_ra, -0.62906467914581298828125_ra } };
        SphericalPointSet<HammersleyPointSet> seq { 5 };
        for ( size_t i = 0; i < 5; ++i ) {
            auto v = seq( i );
            REQUIRE( isApprox( v[0], seq_verif[i].first ) );
            REQUIRE( isApprox( v[1], seq_verif[i].second ) );
        }
    }
}
