#include <Core/Utils/Color.hpp>
#include <Tests.hpp>

#include <algorithm>
#include <functional>
#include <random>

namespace Ra {
namespace Testing {

void run() {
    using namespace Core::Utils;
    Color color1 = Color::Red(), color2 = Color::Green();
    Color white = color1 + color2 + Color::Blue(); // white = 1,1,1,3

    RA_VERIFY( white.rgb().isApprox( Color::White().rgb() ), "Invalid color presets" );

    Scalar alpha = 0.5;
    // interpolate rgba
    Color grey = ( alpha * Color::Black() + ( 1 - alpha ) * white );
    RA_VERIFY( grey.rgb().isApprox( Color::Grey( 0.5 ).rgb() ),
               "Invalid color presets or interpolation 1" );

    // interpolate rgb
    grey = Color::fromRGB( alpha * Color::Black().rgb() + ( 1 - alpha ) * white.rgb() );
    RA_VERIFY( grey.isApprox( Color::Grey( 0.5 ) ), "Invalid color presets or interpolation 2" );

    grey =
        Color::fromRGB( alpha * Color::Black().rgb() + ( 1 - alpha ) * white.rgb(), Scalar( 1 ) );
    RA_VERIFY( grey.isApprox( Color::Grey( 0.5 ) ), "Invalid color presets or interpolation 3" );

    Color grey2 = Color::fromRGB( Eigen::Matrix<Scalar, 3, 1>::Constant( 0.5 ) );
    RA_VERIFY( grey.isApprox( grey2 ), "Conversion from RBG failed" );

    Color grey3;
    grey3 << .5, .5, .5, 1.;
    RA_VERIFY( grey.isApprox( grey3 ), "Conversion from comma operator failed" );

    uint32_t half = 0.5 * 255;
    uint32_t one = 255;
    auto rgba = ( half << 24 ) | ( half << 16 ) | ( half << 8 ) | ( one << 0 );
    auto argb = ( one << 24 ) | ( half << 16 ) | ( half << 8 ) | ( half << 0 );
    RA_VERIFY( grey3.toRGBA32() == rgba, "Conversion to RGBA32 failed" );
    RA_VERIFY( grey3.toARGB32() == argb, "Conversion to ARGB32 failed" );
}
} // namespace Testing
} // namespace Ra

int main( int argc, const char** argv ) {
    using namespace Ra;

    if ( !Testing::init_testing( 1, argv ) )
    {
        return EXIT_FAILURE;
    }

#pragma omp parallel for
    for ( int i = 0; i < Testing::g_repeat; ++i )
    {
        CALL_SUBTEST( ( Testing::run() ) );
    }

    return EXIT_SUCCESS;
}
