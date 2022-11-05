#include <Dataflow/Rendering/Nodes/RenderNodes/GeometryAovsNode.hpp>

#include <Engine/Data/Material.hpp>
#include <Engine/Data/ShaderConfigFactory.hpp>

namespace Ra {
namespace Dataflow {
namespace Rendering {
namespace Nodes {

using namespace Ra::Engine::Data;

GeometryAovsNode::GeometryAovsNode( const std::string& name ) :
    RenderingNode( name, getTypename() ) {
    addInput( m_inObjects );
    m_inObjects->mustBeLinked();
    addInput( m_inCamera );
    m_inCamera->mustBeLinked();

    // Create internal texture
    Ra::Engine::Data::TextureParameters texParams;
    texParams.target = gl::GL_TEXTURE_2D;

    texParams.internalFormat = gl::GL_RGBA32F;
    texParams.format         = gl::GL_RGBA;
    texParams.type           = gl::GL_FLOAT;
    texParams.minFilter      = gl::GL_NEAREST;
    texParams.magFilter      = gl::GL_NEAREST;
    texParams.name           = "world pos";
    m_posInWorldTexture      = new Ra::Engine::Data::Texture( texParams );

    texParams.minFilter    = gl::GL_LINEAR;
    texParams.magFilter    = gl::GL_LINEAR;
    texParams.name         = "world normal";
    m_normalInWorldTexture = new Ra::Engine::Data::Texture( texParams );

    texParams.internalFormat = gl::GL_DEPTH_COMPONENT24;
    texParams.format         = gl::GL_DEPTH_COMPONENT;
    texParams.type           = gl::GL_UNSIGNED_INT;
    texParams.minFilter      = gl::GL_NEAREST;
    texParams.magFilter      = gl::GL_NEAREST;
    texParams.name           = "depth";
    m_depthTexture           = new Ra::Engine::Data::Texture( texParams );

    addOutput( m_outDepthTex, m_depthTexture );
    addOutput( m_outPosInWorldTex, m_posInWorldTexture );
    addOutput( m_outNormalInWorldTex, m_normalInWorldTexture );
}

void GeometryAovsNode::init() {
    m_framebuffer = new globjects::Framebuffer();

    m_nodeState = new globjects::State( globjects::State::DeferredMode );
    m_nodeState->enable( gl::GL_DEPTH_TEST );
    m_nodeState->depthMask( gl::GL_TRUE );
    m_nodeState->depthFunc( gl::GL_LESS );
    m_nodeState->colorMask( gl::GL_TRUE, gl::GL_TRUE, gl::GL_TRUE, gl::GL_TRUE );
    m_nodeState->disable( gl::GL_BLEND );
}

void GeometryAovsNode::destroy() {
    delete m_depthTexture;
    delete m_normalInWorldTexture;
    delete m_posInWorldTexture;

    delete m_nodeState;
    delete m_framebuffer;
}

void GeometryAovsNode::resize( uint32_t width, uint32_t height ) {
    m_depthTexture->resize( width, height );
    m_posInWorldTexture->resize( width, height );
    m_normalInWorldTexture->resize( width, height );

    m_framebuffer->bind();
    m_framebuffer->attachTexture( gl::GL_DEPTH_ATTACHMENT, m_depthTexture->texture() );
    m_framebuffer->attachTexture( gl::GL_COLOR_ATTACHMENT0, m_posInWorldTexture->texture() );
    m_framebuffer->attachTexture( gl::GL_COLOR_ATTACHMENT1, m_normalInWorldTexture->texture() );
    m_framebuffer->unbind();
}

void GeometryAovsNode::buildRenderTechnique( const Ra::Engine::Rendering::RenderObject* ro,
                                             Ra::Engine::Rendering::RenderTechnique& rt ) const {
    std::string resourcesRootDir = m_resourceDir + "Shaders/GeometryAovs/";
    auto mat = const_cast<Ra::Engine::Rendering::RenderObject*>( ro )->getMaterial();
    // Volumes are not used in geomPrepass
    if ( mat->getMaterialAspect() == Ra::Engine::Data::Material::MaterialAspect::MAT_DENSITY ) {
        return;
    }
    if ( auto cfg = Ra::Engine::Data::ShaderConfigurationFactory::getConfiguration(
             { "GeometryAovsNode::" + mat->getMaterialName() } ) ) {
        rt.setConfiguration( *cfg, m_idx );
    }
    else {
        // Build the shader configuration
        Ra::Engine::Data::ShaderConfiguration theConfig {
            { "GeometryAovsNode::" + mat->getMaterialName() },
            m_resourceDir + "Shaders/shader_nolight.vert.glsl",
            resourcesRootDir + "shader.frag.glsl" };
        // add the material interface to the fragment shader
        theConfig.addInclude( "\"" + mat->getMaterialName() + ".glsl\"",
                              Ra::Engine::Data::ShaderType::ShaderType_FRAGMENT );
        // Add to the ShaderConfigManager
        Ra::Engine::Data::ShaderConfigurationFactory::addConfiguration( theConfig );
        // Add to the RenderTechniq
        rt.setConfiguration( theConfig, m_idx );
    }
    rt.setParametersProvider( mat, m_idx );
}

bool GeometryAovsNode::execute() {
    // Get parameters
    // Render objects
    auto& renderObjects = m_inObjects->getData();
    // Cameras
    auto& camera = m_inCamera->getData();

    // Compute the result
    m_framebuffer->bind();

    const gl::GLenum buffers[] = { gl::GL_COLOR_ATTACHMENT0, gl::GL_COLOR_ATTACHMENT1 };
    gl::glDrawBuffers( 2, buffers );

    auto currentState = globjects::State::currentState();
    m_nodeState->apply();

    float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    float clearDepth    = 1.0f;
    gl::glClearBufferfv( gl::GL_COLOR, 0, clearColor );
    gl::glClearBufferfv( gl::GL_COLOR, 1, clearColor );
    gl::glClearBufferfv( gl::GL_DEPTH, 0, &clearDepth );

    Ra::Engine::Data::RenderParameters inPassParams;
    for ( const auto& ro : renderObjects ) {
        ro->render( inPassParams, camera, m_idx );
    }

    currentState->apply();
    m_framebuffer->unbind();

    // Set output port
    // here, they are already set by constructor
    return true;
}

} // namespace Nodes
} // namespace Rendering
} // namespace Dataflow
} // namespace Ra
