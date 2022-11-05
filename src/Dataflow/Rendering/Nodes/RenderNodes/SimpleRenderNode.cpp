#include <Dataflow/Rendering/Nodes/RenderNodes/SimpleRenderNode.hpp>

#include <Engine/Data/Material.hpp>
#include <Engine/Data/ShaderConfigFactory.hpp>

namespace Ra {
namespace Dataflow {
namespace Rendering {
namespace Nodes {

using namespace Ra::Engine::Data;

SimpleRenderNode::SimpleRenderNode( const std::string& name ) :
    RenderingNode( name, getTypename() ) {
    addInput( m_inObjects );
    m_inObjects->mustBeLinked();
    addInput( m_inLights );
    m_inLights->mustBeLinked();
    addInput( m_inCamera );
    m_inCamera->mustBeLinked();

    Ra::Engine::Data::TextureParameters colorTexParams = { name,
                                                           gl::GL_TEXTURE_2D,
                                                           1,
                                                           1,
                                                           1,
                                                           gl::GL_RGBA,
                                                           gl::GL_RGBA32F,
                                                           gl::GL_FLOAT,
                                                           gl::GL_CLAMP_TO_EDGE,
                                                           gl::GL_CLAMP_TO_EDGE,
                                                           gl::GL_CLAMP_TO_EDGE,
                                                           gl::GL_LINEAR,
                                                           gl::GL_LINEAR,
                                                           nullptr };
    m_colorTexture = new Ra::Engine::Data::Texture( colorTexParams );
    addOutput( m_outColorTex, m_colorTexture );

    Ra::Engine::Data::TextureParameters depthTexParams = { "Simple Depth image",
                                                           gl::GL_TEXTURE_2D,
                                                           1,
                                                           1,
                                                           1,
                                                           gl::GL_DEPTH_COMPONENT,
                                                           gl::GL_DEPTH_COMPONENT24,
                                                           gl::GL_UNSIGNED_INT,
                                                           gl::GL_CLAMP_TO_EDGE,
                                                           gl::GL_CLAMP_TO_EDGE,
                                                           gl::GL_CLAMP_TO_EDGE,
                                                           gl::GL_NEAREST,
                                                           gl::GL_NEAREST,
                                                           nullptr };
    m_depthTexture = new Ra::Engine::Data::Texture( depthTexParams );
    addOutput( m_outDepthTex, m_depthTexture );
}

void SimpleRenderNode::init() {
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
    m_nodeState->depthMask( gl::GL_TRUE );
    m_nodeState->depthFunc( gl::GL_LEQUAL );
    m_nodeState->colorMask( gl::GL_TRUE, gl::GL_TRUE, gl::GL_TRUE, gl::GL_TRUE );
    m_nodeState->blendFunc( gl::GL_ONE, gl::GL_ONE );
    m_nodeState->disable( gl::GL_BLEND );
}

void SimpleRenderNode::destroy() {
    delete m_framebuffer;
    delete m_colorTexture;
    delete m_depthTexture;
    delete m_blankAO;

    delete m_nodeState;
}

void SimpleRenderNode::resize( uint32_t width, uint32_t height ) {
    m_colorTexture->resize( width, height );
    m_depthTexture->resize( width, height );
    m_framebuffer->bind();
    m_framebuffer->attachTexture( gl::GL_DEPTH_ATTACHMENT, m_depthTexture->texture() );
    m_framebuffer->attachTexture( gl::GL_COLOR_ATTACHMENT0, m_colorTexture->texture() );
    m_framebuffer->unbind();
}

void SimpleRenderNode::buildRenderTechnique( const Ra::Engine::Rendering::RenderObject* ro,
                                             Ra::Engine::Rendering::RenderTechnique& rt ) const {

    auto mat = const_cast<Ra::Engine::Rendering::RenderObject*>( ro )->getMaterial();
    // Volumes are not used in EnvLightPass
    if ( mat->getMaterialAspect() == Ra::Engine::Data::Material::MaterialAspect::MAT_DENSITY ) {
        return;
    }

    if ( auto cfg = Ra::Engine::Data::ShaderConfigurationFactory::getConfiguration(
             { "LocalLightNode::" + mat->getMaterialName() } ) ) {
        rt.setConfiguration( *cfg, m_idx );
    }
    else {
        std::string resourcesRootDir = m_resourceDir + "Shaders/LocalLightNode/";
        // Build the shader configuration
        Ra::Engine::Data::ShaderConfiguration theConfig {
            { "LocalLightNode::" + mat->getMaterialName() },
            m_resourceDir + "Shaders/shader.vert.glsl",
            resourcesRootDir + "shader.frag.glsl" };
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

bool SimpleRenderNode::execute() {
    // Get parameters
    // Render objects
    auto& renderObjects = m_inObjects->getData();
    // Cameras
    auto& camera = m_inCamera->getData();
    // Lights
    auto& lights = m_inLights->getData();

    // Compute the result
    m_framebuffer->bind();

    const gl::GLenum buffers[] = { gl::GL_COLOR_ATTACHMENT0 };
    gl::glDrawBuffers( 1, buffers );

    auto currentState = globjects::State::currentState();
    m_nodeState->apply();

    float clearColor[4] = { 0.02f, 0.03f, 0.04f, 0.0f };
    float clearDepth    = 1.0f;
    gl::glClearBufferfv( gl::GL_COLOR, 0, clearColor );
    gl::glClearBufferfv( gl::GL_DEPTH, 0, &clearDepth );

    if ( lights.size() > 0 ) {
        bool first_light = true;
        for ( const auto& l : lights ) {
            Ra::Engine::Data::RenderParameters inPassParams;
            inPassParams.addParameter( "amb_occ_sampler", m_blankAO );
            l->getRenderParameters( inPassParams );
            for ( const auto& ro : renderObjects ) {
                ro->render( inPassParams, camera, m_idx );
            }
            if ( first_light ) {
                // break;
                first_light = false;
                gl::glEnable( gl::GL_BLEND );
                gl::glBlendFunc( gl::GL_ONE, gl::GL_ONE );
                gl::glDepthMask( gl::GL_FALSE );
            }
        }
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
