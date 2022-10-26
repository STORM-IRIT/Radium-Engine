
#include <Core/Tasks/Task.hpp>
#include <Core/Tasks/TaskQueue.hpp>
#include <Core/Utils/Log.hpp>
#include <Engine/Data/Texture.hpp>
#include <Engine/RadiumEngine.hpp>

#include <glbinding/gl/enum.h>
#include <globjects/Texture.h>

#include <cmath>

namespace Ra {
namespace Engine {
namespace Data {
using namespace Core::Utils; // log

Texture::Texture( const TextureParameters& texParameters ) :
    m_textureParameters { texParameters },
    m_texture { nullptr },
    m_isMipMapped { false },
    m_isLinear { false } {}

Texture::~Texture() {
    if ( m_updateImageTaskId.isValid() ) {
        RadiumEngine::getInstance()->removeGpuTask( m_updateImageTaskId );
    }

    if ( m_updateSamplerTaskId.isValid() ) {
        RadiumEngine::getInstance()->removeGpuTask( m_updateSamplerTaskId );
    }
}

void Texture::initialize( bool linearize ) {
    if ( !isSupportedTarget() ) return;
    // Transform texels if needed
    if ( linearize ) { this->linearize(); }

    computeIsMipMappedFlag();

    // Update the sampler parameters
    registerUpdateSamplerParametersTask();

    // upload texture to the GPU
    registerUpdateImageDataTask();
}

void Texture::updateData( std::shared_ptr<void> newData ) {
    // register gpu task to update opengl representation before next rendering
    std::lock_guard<std::mutex> lock( m_updateMutex );
    m_textureParameters.image.texels = newData;
    registerUpdateImageDataTask();
}

void Texture::resize( size_t w, size_t h, size_t d, std::shared_ptr<void> pix ) {
    m_textureParameters.image.width  = w;
    m_textureParameters.image.height = h;
    m_textureParameters.image.depth  = d;
    m_textureParameters.image.texels = pix;
    if ( createTexture() ) {
        computeIsMipMappedFlag();
        sendSamplerParametersToGPU();
        sendImageDataToGPU();
    }
    else { sendImageDataToGPU(); }
}

void Texture::setParameters( const TextureParameters& textureParameters ) {
    m_updateMutex.lock();
    bool test1 = ( textureParameters.sampler != m_textureParameters.sampler );
    m_updateMutex.unlock();
    if ( test1 ) setSamplerParameters( textureParameters.sampler );

    m_updateMutex.lock();
    bool test2 = ( textureParameters.image != m_textureParameters.image );
    m_updateMutex.unlock();
    if ( test2 ) setImageParameters( textureParameters.image );
}

void Texture::setImageParameters( const ImageParameters& imageParameters ) {
    std::lock_guard<std::mutex> lock( m_updateMutex );
    m_textureParameters.image = imageParameters;
    registerUpdateImageDataTask();
}

void Texture::setSamplerParameters( const SamplerParameters& samplerParameters ) {
    std::lock_guard<std::mutex> lock( m_updateMutex );
    m_textureParameters.sampler = samplerParameters;
    registerUpdateSamplerParametersTask();
}

void Texture::bind( int unit ) {
    if ( unit >= 0 ) { m_texture->bindActive( uint( unit ) ); }
    else { m_texture->bind(); }
}

void Texture::bindImageTexture( int unit,
                                GLint level,
                                GLboolean layered,
                                GLint layer,
                                GLenum access ) {
    m_texture->bindImageTexture(
        uint( unit ), level, layered, layer, access, m_textureParameters.image.internalFormat );
}

void Texture::linearize() {
    // Only RGB and RGBA texture contains color information
    // (others are not really colors and must be managed explicitly by the user)
    uint numComp  = 0;
    bool hasAlpha = false;
    switch ( m_textureParameters.image.format ) {
        // RED texture store a gray scale color. Verify if we need to convert
    case GL_RED:
        numComp = 1;
        break;
    case GL_RGB:
        numComp = 3;
        break;
    case GL_RGBA:
        numComp  = 4;
        hasAlpha = true;
        break;
    default:
        LOG( logERROR ) << "Textures with format " << m_textureParameters.image.format
                        << " can't be linearized." << m_textureParameters.name;
        return;
    }
    if ( m_textureParameters.image.type == GL_TEXTURE_CUBE_MAP ) {
        linearizeCubeMap( numComp, hasAlpha );
    }
    else {
        sRGBToLinearRGB( reinterpret_cast<uint8_t*>( m_textureParameters.image.texels.get() ),
                         numComp,
                         hasAlpha );
    }
}

bool Texture::isSupportedTarget() {
    if ( ( m_textureParameters.image.target != GL_TEXTURE_1D ) &&
         ( m_textureParameters.image.target != GL_TEXTURE_2D ) &&
         ( m_textureParameters.image.target != GL_TEXTURE_RECTANGLE ) &&
         ( m_textureParameters.image.target != GL_TEXTURE_3D ) &&
         ( m_textureParameters.image.target != GL_TEXTURE_CUBE_MAP ) ) {
        LOG( logERROR ) << "Texture of type " << m_textureParameters.image.target
                        << " must be generated explicitly!";
        return false;
    }
    return true;
}

void Texture::computeIsMipMappedFlag() {
    m_isMipMapped = !( m_textureParameters.sampler.minFilter == GL_NEAREST ||
                       m_textureParameters.sampler.minFilter == GL_LINEAR );
}

bool Texture::createTexture() {
    if ( m_texture == nullptr ) {
        m_texture = globjects::Texture::create( m_textureParameters.image.target );
        GL_CHECK_ERROR;
        return true;
    }
    return false;
}

void Texture::registerUpdateImageDataTask() {
    // register gpu task to update opengl representation before next rendering
    if ( m_updateImageTaskId.isInvalid() ) {
        auto taskFunc = [this]() {
            std::lock_guard<std::mutex> taskLock( m_updateMutex );
            // Generate OpenGL texture
            this->createTexture();
            this->sendImageDataToGPU();
            m_updateImageTaskId = Core::TaskQueue::TaskId::Invalid();
        };
        auto task           = std::make_unique<Core::FunctionTask>( taskFunc, getName() );
        m_updateImageTaskId = RadiumEngine::getInstance()->addGpuTask( std::move( task ) );
    }
}

void Texture::registerUpdateSamplerParametersTask() {
    if ( m_updateSamplerTaskId.isInvalid() ) {
        auto taskFunc = [this]() {
            std::lock_guard<std::mutex> taskLock( m_updateMutex );
            // Generate OpenGL texture
            this->createTexture();
            this->sendSamplerParametersToGPU();
            m_updateSamplerTaskId = Core::TaskQueue::TaskId::Invalid();
        };
        auto task             = std::make_unique<Core::FunctionTask>( taskFunc, getName() );
        m_updateSamplerTaskId = RadiumEngine::getInstance()->addGpuTask( std::move( task ) );
    }
}

void Texture::sendImageDataToGPU() {
    CORE_ASSERT( m_texture != nullptr, "Cannot update non initialized texture" );
    switch ( m_texture->target() ) {
    case GL_TEXTURE_1D: {
        m_texture->image1D( 0,
                            m_textureParameters.image.internalFormat,
                            GLsizei( m_textureParameters.image.width ),
                            0,
                            m_textureParameters.image.format,
                            m_textureParameters.image.type,
                            m_textureParameters.image.texels.get() );
        GL_CHECK_ERROR
    } break;
    case GL_TEXTURE_2D:
    case GL_TEXTURE_RECTANGLE: {
        m_texture->image2D( 0,
                            m_textureParameters.image.internalFormat,
                            GLsizei( m_textureParameters.image.width ),
                            GLsizei( m_textureParameters.image.height ),
                            0,
                            m_textureParameters.image.format,
                            m_textureParameters.image.type,
                            m_textureParameters.image.texels.get() );
        GL_CHECK_ERROR
    } break;
    case GL_TEXTURE_3D: {
        m_texture->image3D( 0,
                            m_textureParameters.image.internalFormat,
                            GLsizei( m_textureParameters.image.width ),
                            GLsizei( m_textureParameters.image.height ),
                            GLsizei( m_textureParameters.image.depth ),
                            0,
                            m_textureParameters.image.format,
                            m_textureParameters.image.type,
                            m_textureParameters.image.texels.get() );
        GL_CHECK_ERROR
    } break;
    case GL_TEXTURE_CUBE_MAP: {
        // Load the 6 faces of the cube-map

        m_texture->bind();
        // track globjects updates that will hopefully support direct loading of
        // cube-maps https://github.com/cginternals/globjects/issues/368
        gl::glTexImage2D( gl::GL_TEXTURE_CUBE_MAP_POSITIVE_X,
                          0,
                          m_textureParameters.image.internalFormat,
                          GLsizei( m_textureParameters.image.width ),
                          GLsizei( m_textureParameters.image.height ),
                          0,
                          m_textureParameters.image.format,
                          m_textureParameters.image.type,
                          m_textureParameters.image.cubeMap[0].get() );
        gl::glTexImage2D( gl::GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                          0,
                          m_textureParameters.image.internalFormat,
                          GLsizei( m_textureParameters.image.width ),
                          GLsizei( m_textureParameters.image.height ),
                          0,
                          m_textureParameters.image.format,
                          m_textureParameters.image.type,
                          m_textureParameters.image.cubeMap[1].get() );

        gl::glTexImage2D( gl::GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
                          0,
                          m_textureParameters.image.internalFormat,
                          GLsizei( m_textureParameters.image.width ),
                          GLsizei( m_textureParameters.image.height ),
                          0,
                          m_textureParameters.image.format,
                          m_textureParameters.image.type,
                          m_textureParameters.image.cubeMap[2].get() );
        gl::glTexImage2D( gl::GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
                          0,
                          m_textureParameters.image.internalFormat,
                          GLsizei( m_textureParameters.image.width ),
                          GLsizei( m_textureParameters.image.height ),
                          0,
                          m_textureParameters.image.format,
                          m_textureParameters.image.type,
                          m_textureParameters.image.cubeMap[3].get() );

        gl::glTexImage2D( gl::GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
                          0,
                          m_textureParameters.image.internalFormat,
                          GLsizei( m_textureParameters.image.width ),
                          GLsizei( m_textureParameters.image.height ),
                          0,
                          m_textureParameters.image.format,
                          m_textureParameters.image.type,
                          m_textureParameters.image.cubeMap[4].get() );
        gl::glTexImage2D( gl::GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
                          0,
                          m_textureParameters.image.internalFormat,
                          GLsizei( m_textureParameters.image.width ),
                          GLsizei( m_textureParameters.image.height ),
                          0,
                          m_textureParameters.image.format,
                          m_textureParameters.image.type,
                          m_textureParameters.image.cubeMap[5].get() );

        m_texture->unbind();
        GL_CHECK_ERROR
    } break;
    default: {
        CORE_ASSERT( 0, "Unsupported texture type ?" );
    } break;
    }
    // Generate mip-map if needed.
    if ( m_isMipMapped ) { m_texture->generateMipmap(); }

    GL_CHECK_ERROR;
}

// let the compiler warn about case fallthrough
void Texture::sendSamplerParametersToGPU() {
    switch ( m_texture->target() ) {
    case GL_TEXTURE_CUBE_MAP:
    case GL_TEXTURE_3D:
        m_texture->setParameter( GL_TEXTURE_WRAP_R, m_textureParameters.sampler.wrapP );
        GL_CHECK_ERROR;
        [[fallthrough]];
    case GL_TEXTURE_2D:
    case GL_TEXTURE_RECTANGLE:
        m_texture->setParameter( GL_TEXTURE_WRAP_T, m_textureParameters.sampler.wrapT );
        GL_CHECK_ERROR;
        [[fallthrough]];
    case GL_TEXTURE_1D:
        m_texture->setParameter( GL_TEXTURE_WRAP_S, m_textureParameters.sampler.wrapS );
        GL_CHECK_ERROR;
        break;
    default:
        break;
    }
    m_texture->setParameter( GL_TEXTURE_MIN_FILTER, m_textureParameters.sampler.minFilter );
    GL_CHECK_ERROR;
    m_texture->setParameter( GL_TEXTURE_MAG_FILTER, m_textureParameters.sampler.magFilter );
    GL_CHECK_ERROR;
}

/// \todo template by texels type
void Texture::sRGBToLinearRGB( uint8_t* texels, uint numComponent, bool hasAlphaChannel ) {
    std::lock_guard<std::mutex> lock( m_updateMutex );
    if ( !m_isLinear ) {
        m_isLinear = true;
        // auto linearize = [gamma](float in)-> float {
        auto linearize = []( uint8_t in ) -> uint8_t {
            // Constants are described at https://en.wikipedia.org/wiki/SRGB
            float c = float( in ) / 255;
            if ( c < 0.04045 ) { c = c / 12.92f; }
            else { c = std::pow( ( ( c + 0.055f ) / ( 1.055f ) ), 2.4f ); }
            return uint8_t( c * 255 );
        };
        uint numValues = hasAlphaChannel ? numComponent - 1 : numComponent;
#pragma omp parallel for
        for ( int i = 0;
              i < int( m_textureParameters.image.width * m_textureParameters.image.height *
                       m_textureParameters.image.depth );
              ++i ) {
            // Convert each R or RGB value while keeping alpha unchanged
            for ( uint p = i * numComponent; p < i * numComponent + numValues; ++p ) {
                texels[p] = linearize( texels[p] );
            }
        }
    }
}

void Texture::linearizeCubeMap( uint numComponent, bool hasAlphaChannel ) {
    if ( m_textureParameters.image.type == gl::GLenum::GL_UNSIGNED_BYTE ) {
        /// Only unsigned byte texture could be linearized. Considering other formats where
        /// already linear
        for ( int i = 0; i < 6; ++i ) {
            sRGBToLinearRGB(
                reinterpret_cast<uint8_t*>( m_textureParameters.image.cubeMap[i].get() ),
                numComponent,
                hasAlphaChannel );
        }
    }
}

} // namespace Data
} // namespace Engine
} // namespace Ra
