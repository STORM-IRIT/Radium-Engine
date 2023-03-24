#include <Dataflow/Rendering/Nodes/RenderNodes/EnvironmentLightingNode.hpp>

#include <Engine/Data/Material.hpp>
#include <Engine/Data/ShaderConfigFactory.hpp>

namespace Ra {
namespace Dataflow {
namespace Rendering {
namespace Nodes {

using namespace Ra::Engine::Data;

EnvironmentLightingNode::EnvironmentLightingNode( const std::string& name ) :
    RenderingNode( name, getTypename() ) {

    addInput( m_inObjects );
    m_inObjects->mustBeLinked();
    addInput( m_inCamera );
    m_inCamera->mustBeLinked();

    addInput( m_inEnvironment );
    // if no envmap available, the node will be pass_through

    // TODO, allow to not provide the two following inputs, then using internal textures ?
    addInput( m_inColor );
    m_inColor->mustBeLinked();
    addInput( m_inDepth );
    m_inDepth->mustBeLinked();

    addInput( m_inAo );
    addOutput( m_outColor, m_colorTexture );
}

void EnvironmentLightingNode::init() {
    m_framebuffer = new globjects::Framebuffer();

    float blankAO[4] = { 1.f, 1.f, 1.f, 1.f };
    Ra::Engine::Data::TextureParameters texParams;
    texParams.target         = gl::GL_TEXTURE_2D;
    texParams.width          = 1;
    texParams.height         = 1;
    texParams.internalFormat = gl::GL_RGBA32F;
    texParams.format         = gl::GL_RGBA;
    texParams.type           = gl::GL_FLOAT;
    texParams.minFilter      = gl::GL_NEAREST;
    texParams.magFilter      = gl::GL_NEAREST;
    texParams.name           = "Blank AO";
    texParams.texels         = &blankAO;
    m_blankAO                = new Ra::Engine::Data::Texture( texParams );
    m_blankAO->initializeGL();

    m_nodeState = new globjects::State( globjects::State::DeferredMode );
    m_nodeState->enable( gl::GL_DEPTH_TEST );
    m_nodeState->depthFunc( gl::GL_LEQUAL );
    m_nodeState->depthMask( gl::GL_FALSE );
    m_nodeState->colorMask( gl::GL_TRUE, gl::GL_TRUE, gl::GL_TRUE, gl::GL_TRUE );
    // m_nodeState->blendFunc( gl::GL_ONE, gl::GL_DST_ALPHA );
    m_nodeState->blendFuncSeparate( gl::GL_ONE, gl::GL_DST_ALPHA, gl::GL_ONE, gl::GL_ZERO );
    m_nodeState->enable( gl::GL_BLEND );
}

void EnvironmentLightingNode::destroy() {
    delete m_framebuffer;
    delete m_blankAO;

    delete m_nodeState;
}

void EnvironmentLightingNode::resize( uint32_t, uint32_t ) {}

void EnvironmentLightingNode::buildRenderTechnique(
    const Ra::Engine::Rendering::RenderObject* ro,
    Ra::Engine::Rendering::RenderTechnique& rt ) const {

    auto mat = const_cast<Ra::Engine::Rendering::RenderObject*>( ro )->getMaterial();
    // Volumes are not (yet) managed by EnvironmentLightingNode
    if ( mat->getMaterialAspect() == Ra::Engine::Data::Material::MaterialAspect::MAT_DENSITY ) {
        return;
    }
    if ( auto cfg = Ra::Engine::Data::ShaderConfigurationFactory::getConfiguration(
             { "EnvironmentLightingNode::" + mat->getMaterialName() } ) ) {
        rt.setConfiguration( *cfg, m_idx );
    }
    else {
        std::string nodeShaderDir = m_resourceDir + "Shaders/EnvLightNode/";
        // Build the shader configuration
        Ra::Engine::Data::ShaderConfiguration theConfig {
            { "EnvironmentLightingNode::" + mat->getMaterialName() },
            m_resourceDir + "Shaders/shader_nolight.vert.glsl",
            nodeShaderDir + "shader.frag.glsl" };
        // add the material interface to the fragment shader
        theConfig.addInclude( "\"" + mat->getMaterialName() + ".glsl\"",
                              Ra::Engine::Data::ShaderType::ShaderType_FRAGMENT );
        // Add to the ShaderConfigManager
        Ra::Engine::Data::ShaderConfigurationFactory::addConfiguration( theConfig );
        // Add to the RenderTechnique
        rt.setConfiguration( theConfig, m_idx );
    }
    rt.setParametersProvider( mat, m_idx );
}

bool EnvironmentLightingNode::execute() {
    // Get parameters
    // Render objects
    auto& renderObjects = m_inObjects->getData();
    // Cameras
    auto& camera = m_inCamera->getData();

    // Color tex
    m_colorTexture = &m_inColor->getData();
    m_outColor->setData( m_colorTexture );

    // Depth buffer
    auto& depthBuffer = m_inDepth->getData();

    // SSAO texture
    auto aoTexture = m_inAo->isLinked() ? &m_inAo->getData() : m_blankAO;
    Ra::Engine::Data::RenderParameters inPassParams;
    inPassParams.addParameter( "amb_occ_sampler", aoTexture );

    // Environment map
    auto envmap = m_inEnvironment->isLinked() ? m_inEnvironment->getData().get() : nullptr;
    if ( !envmap ) { return true; }

    // TODO, verify why this is needed ....
    envmap->updateGL();

    inPassParams.addParameter( "redShCoeffs", envmap->getShMatrix( 0 ) );
    inPassParams.addParameter( "greenShCoeffs", envmap->getShMatrix( 1 ) );
    inPassParams.addParameter( "blueShCoeffs", envmap->getShMatrix( 2 ) );
    inPassParams.addParameter( "envTexture", envmap->getEnvironmentTexture() );
    inPassParams.addParameter( "envStrength", envmap->getStrength() );
    int numLod = std::log2( std::min( envmap->getEnvironmentTexture()->width(),
                                      envmap->getEnvironmentTexture()->height() ) );
    inPassParams.addParameter( "numLod", numLod );

    // Compute the result
    m_framebuffer->bind();
    m_framebuffer->attachTexture( gl::GL_DEPTH_ATTACHMENT, depthBuffer.texture() );
    m_framebuffer->attachTexture( gl::GL_COLOR_ATTACHMENT0, m_colorTexture->texture() );

    const gl::GLenum buffers[] = { gl::GL_COLOR_ATTACHMENT0 };
    gl::glDrawBuffers( 1, buffers );

    auto currentState = globjects::State::currentState();
    m_nodeState->apply();
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
