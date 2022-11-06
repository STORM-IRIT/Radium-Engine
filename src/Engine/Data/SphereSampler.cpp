#include <Core/Random/RandomPointSet.hpp>
#include <Engine/Data/SphereSampler.hpp>

namespace Ra {
namespace Engine {
namespace Data {

using namespace Core::Random;

SphereSampler::SphereSampler( SamplingMethod method, int level ) {
    std::function<Ra::Core::Vector3( int )> generator;
    switch ( method ) {
    case SamplingMethod::FIBONACCI: {
        auto g     = SphericalPointSet<FibonacciPointSet>( level );
        m_nbPoints = g.range();
        generator  = std::move( g );
    } break;
    case SamplingMethod::HAMMERSLEY: {
        auto g     = SphericalPointSet<HammersleyPointSet>( level );
        m_nbPoints = g.range();
        generator  = std::move( g );
    } break;
    case SamplingMethod::RANDOM: {
        auto g     = SphericalPointSet<MersenneTwisterPointSet>( level );
        m_nbPoints = g.range();
        generator  = std::move( g );
    } break;
    case SamplingMethod::GEODESIC:
        // not yet implemented, resolve as default
        //    default: // for the moment, implement GEODESIC
        {
            auto g     = SphericalPointSet<MersenneTwisterPointSet>( level );
            m_nbPoints = g.range();
            generator  = std::move( g );
        }
        break;
    }

    std::mt19937 seq( 0 );
    std::uniform_real_distribution<Scalar> random( 0._ra, 1._ra );

    m_points.reserve( size_t( m_nbPoints ) );
    for ( auto i = 0; i < m_nbPoints; ++i ) {
        auto pt = generator( i );
        auto d  = random( seq );
        // lerp will only be available on C++20 and after ...
        // d = std::lerp(0.1, 1.0, d*d);
        d *= d;
        d = 0.1_ra + d * 0.9_ra;
        m_points.emplace_back( pt[0], pt[1], pt[2], d );
    }
}

Ra::Engine::Data::Texture* SphereSampler::asTexture() {
    using namespace gl;
    if ( m_texture != nullptr ) { return m_texture.get(); }

    Ra::Engine::Data::TextureParameters texparams;
    texparams.name           = "Directional samples";
    texparams.width          = size_t( m_nbPoints );
    texparams.height         = 1;
    texparams.target         = GL_TEXTURE_RECTANGLE;
    texparams.minFilter      = GL_NEAREST;
    texparams.magFilter      = GL_NEAREST;
    texparams.internalFormat = GL_RGBA32F;
    texparams.format         = GL_RGBA;
    texparams.type           = GL_FLOAT;
    texparams.texels         = m_points.data();
    m_texture                = std::make_unique<Ra::Engine::Data::Texture>( texparams );
    m_texture->initializeGL();
    return m_texture.get();
}

} // namespace Data
} // namespace Engine
} // namespace Ra
