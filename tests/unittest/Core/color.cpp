#include <Core/Utils/Color.hpp>
#include <catch2/catch.hpp>

TEST_CASE( "Core/Utils/Color", "[Core][Core/Utils][Color]" ) {
    using namespace Ra::Core::Utils;

    Scalar alpha = 0.5;

    Color color1 = Color::Red(), color2 = Color::Green();
    Color white =
        Color::fromRGB( color1.rgb() + color2.rgb() + Color::Blue().rgb() ); // white = 1,1,1,1
    Color grey = ( alpha * Color::Black() + ( 1 - alpha ) * white );

    SECTION( "Test color presets" ) {
        REQUIRE( white.rgb().isApprox( Color::White().rgb() ) );
        REQUIRE( white.hasValidAlpha() );

        Color white2 = color1 + color2 + Color::Blue(); // white = 1,1,1,3
        REQUIRE( !white2.hasValidAlpha() );

        REQUIRE( grey.isApprox( Color::Grey( 0.5 ) ) );
    }

    SECTION( "Test color presets and interpolation" ) {

        // interpolate rgba
        REQUIRE( grey.rgb().isApprox( Color::Grey( 0.5 ).rgb() ) );

        // interpolate rgb
        grey = Color::fromRGB( alpha * Color::Black().rgb() + ( 1 - alpha ) * white.rgb() );
        REQUIRE( grey.isApprox( Color::Grey( 0.5 ) ) );

        grey = Color::fromRGB( alpha * Color::Black().rgb() + ( 1 - alpha ) * white.rgb(),
                               Scalar( 1 ) );
        REQUIRE( grey.isApprox( Color::Grey( 0.5 ) ) );
    }
    SECTION( "Test color conversion" ) {
        Color grey2 = Color::fromRGB( Eigen::Matrix<Scalar, 3, 1>::Constant( 0.5 ) );

        // Conversion from RBG failed
        REQUIRE( grey.isApprox( grey2 ) );

        Color grey3;
        grey3 << .5, .5, .5, 1.;
        // Conversion from comma operator failed
        REQUIRE( grey.isApprox( grey3 ) );

        uint32_t half = uint32_t( 0.5 * 255 );
        uint32_t one  = 255;
        auto rgba     = ( half << 24 ) | ( half << 16 ) | ( half << 8 ) | ( one << 0 );
        auto argb     = ( one << 24 ) | ( half << 16 ) | ( half << 8 ) | ( half << 0 );

        // Conversion to RGBA32 failed
        REQUIRE( grey3.toRGBA32() == rgba );
        // Conversion to ARGB32 failed
        REQUIRE( grey3.toARGB32() == argb );
    }
}
