#include "Data/Texture.hpp"
#include <catch2/catch.hpp>

#include <Core/Asset/Image.hpp>

using namespace Ra::Engine::Data;
using namespace Ra::Core::Asset;
using namespace gl;

TEST_CASE( "Engine/Data/Texture", "[Engine][Engine/Data][Texture]" ) {

    constexpr size_t width     = 32;
    constexpr size_t height    = 8;
    constexpr size_t nChannels = 1;
    constexpr size_t size      = width * height * nChannels;
    ImageSpec imgSpec( width, height, nChannels, TypeUInt8 );
    unsigned char data[size] = { 0 };
    for ( auto i = 0u; i < size; ++i ) {
        data[i] = i % 256;
    }
    auto img = std::make_shared<Image>( imgSpec, data, size );

    TextureParameters textureParameters { "procedural",
                                          GL_TEXTURE_2D,
                                          width,
                                          height,
                                          1,
                                          GL_RED,
                                          GL_R8,
                                          GL_UNSIGNED_BYTE,
                                          GL_CLAMP_TO_EDGE,
                                          GL_CLAMP_TO_EDGE,
                                          GL_CLAMP_TO_EDGE,
                                          GL_LINEAR,
                                          GL_LINEAR,
                                          nullptr };

    Texture texture( textureParameters );
    texture.attachImage( img );
    // todo: check if the texture shown by OpenGL is the same as image
    texture.detachImage();
}
