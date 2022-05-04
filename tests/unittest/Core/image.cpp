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
    REQUIRE( img.getAge() == 1 );
    REQUIRE( img.getSizeData() == size );
    REQUIRE( img.getWidth() == width );
    REQUIRE( img.getHeight() == height );
    REQUIRE( img.getNChannels() == nChannels );
    {
        auto* data = img.getData();
        for ( auto i = 0u; i < size; ++i ) {
            REQUIRE( ( (unsigned char*)data )[i] == i % 256 );
        }
    }

    unsigned char data12[size] = { 0 };
    for ( auto i = 0u; i < size; ++i ) {
        data12[i] = ( 256 - i ) % 256;
    }
    img.update( data12 );
    {
        auto* data = img.getData();
        for ( auto i = 0u; i < size; ++i ) {
            REQUIRE( ( (unsigned char*)data )[i] == ( 256 - i ) % 256 );
        }
    }

    img.update( data12, size );
    {
        auto* data = img.getData();
        for ( auto i = 0u; i < size; ++i ) {
            REQUIRE( ( (unsigned char*)data )[i] == ( 256 - i ) % 256 );
        }
    }

    constexpr size_t width2    = 24;
    constexpr size_t height2   = 16;
    constexpr size_t size2     = width2 * height2 * nChannels;
    unsigned char data2[size2] = { 0 };

    img.resize( width2, height2, data2 );
    REQUIRE( img.getSizeData() == size2 );
    REQUIRE( img.getWidth() == width2 );
    REQUIRE( img.getHeight() == height2 );

    img.resize( width, height, data, size );
    REQUIRE( img.getSizeData() == size );
    REQUIRE( img.getWidth() == width );
    REQUIRE( img.getHeight() == height );

    REQUIRE( img.getAge() == 5 );
    REQUIRE( img.getNChannels() == nChannels );
    {
        auto* data = img.getData();
        for ( auto i = 0u; i < size; ++i ) {
            REQUIRE( ( (unsigned char*)data )[i] == i % 256 );
        }
    }

    // check if image modify user data
    for ( auto i = 0u; i < size; ++i ) {
        REQUIRE( ( (unsigned char*)data )[i] == i % 256 );
        REQUIRE( ( (unsigned char*)data12 )[i] == ( 256 - i ) % 256 );
        REQUIRE( ( (unsigned char*)data2 )[i] == 0 % 256 );
    }
}
