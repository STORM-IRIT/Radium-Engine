#include <Core/Asset/Image.hpp>
#include <catch2/catch.hpp>

TEST_CASE( "Core/Asset/Image", "[Core][Core/Image][Image]" ) {
    using namespace Ra::Core::Asset;

    constexpr size_t width     = 32;
    constexpr size_t height    = 8;
    constexpr size_t nChannels = 1;
    constexpr size_t size      = width * height * nChannels;
    ImageSpec imgSpec( width, height, nChannels, TypeUInt8 );
    unsigned char data[size] = { 0 };
    for ( auto i = 0u; i < size; ++i ) {
        data[i] = i % 256;
    }
    Image img( imgSpec, data, size );

    auto* data2 = img.getData();
    for ( auto i = 0u; i < size; ++i ) {
        REQUIRE( ( (unsigned char*)data2 )[i] == i % 256 );
    }

    REQUIRE( img.getAge() == 1 );
    REQUIRE( img.getSizeData() == size );
    REQUIRE( img.getWidth() == width );
    REQUIRE( img.getHeight() == height );
    REQUIRE( img.getNChannels() == nChannels );
}
