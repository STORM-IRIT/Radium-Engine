#include <Dataflow/Rendering/Nodes/RenderNodes/TransparentLocalLightingNode.hpp>

#include <Engine/Data/Material.hpp>
#include <Engine/Data/Mesh.hpp>
#include <Engine/Data/ShaderConfigFactory.hpp>
#include <Engine/Data/ShaderProgramManager.hpp>

namespace Ra {
namespace Dataflow {
namespace Rendering {
namespace Nodes {

using namespace Ra::Engine::Data;

TransparentLocalLightingNode::TransparentLocalLightingNode( const std::string& name ) :
    RenderingNode( name, getTypename() ) {

    addInput( m_inObjects );
    m_inObjects->mustBeLinked();
    addInput( m_inLights );
    m_inLights->mustBeLinked();
    addInput( m_inCamera );
    m_inCamera->mustBeLinked();

    // TODO, allow to not provide the two following inputs, then using internal textures ?
    addInput( m_inColor );
    m_inColor->mustBeLinked();
    addInput( m_inDepth );
    m_inDepth->mustBeLinked();

    addOutput( m_outColor, m_colorTexture );
    addOutput( m_outRevealage, m_revealageTexture );
    addOutput( m_outAccumulation, m_accumulationTexture );
}

void TransparentLocalLightingNode::init() {
    m_framebuffer    = new globjects::Framebuffer();
    m_oitFramebuffer = new globjects::Framebuffer();

    Ra::Engine::Data::TextureParameters texParams;
    texParams.target         = gl::GL_TEXTURE_2D;
    texParams.minFilter      = gl::GL_LINEAR;
    texParams.magFilter      = gl::GL_LINEAR;
    texParams.internalFormat = gl::GL_RGBA32F;
    texParams.format         = gl::GL_RGBA;
    texParams.type           = gl::GL_FLOAT;
    texParams.name           = "Accumulation";
    m_accumulationTexture    = new Ra::Engine::Data::Texture( texParams );
    texParams.name           = "Revealage";
    m_revealageTexture       = new Ra::Engine::Data::Texture( texParams );

    m_outRevealage->setData( m_revealageTexture );
    m_outAccumulation->setData( m_accumulationTexture );

    m_nodeState = new globjects::State( globjects::State::DeferredMode );
    m_nodeState->enable( gl::GL_DEPTH_TEST );
    m_nodeState->depthFunc( gl::GL_LEQUAL );
    m_nodeState->depthMask( gl::GL_FALSE );
    m_nodeState->colorMask( gl::GL_TRUE, gl::GL_TRUE, gl::GL_TRUE, gl::GL_TRUE );
    // no blendEquation in globjects::State
    // m_nodeState->blendEquation(gl::GL_FUNC_ADD);
    // no blendFunci  in globjects::State
    // m_nodeState->blendFunci( 0, gl::GL_ONE, gl::GL_ONE );
    // m_nodeState->blendFunci( 1, gl::GL_ZERO, gl::GL_ONE_MINUS_SRC_ALPHA );
    m_nodeState->enable( gl::GL_BLEND );

    m_composeState = new globjects::State( globjects::State::DeferredMode );
    m_composeState->disable( gl::GL_DEPTH_TEST );
    m_composeState->depthMask( gl::GL_FALSE );
    m_composeState->colorMask( gl::GL_TRUE, gl::GL_TRUE, gl::GL_TRUE, gl::GL_TRUE );
    m_composeState->blendFunc( gl::GL_ONE_MINUS_SRC_ALPHA, gl::GL_SRC_ALPHA );
    m_composeState->enable( gl::GL_BLEND );
}

bool TransparentLocalLightingNode::initInternalShaders() {
    if ( !m_hasShaders ) {
        Ra::Core::Geometry::TriangleMesh mesh =
            Ra::Core::Geometry::makeZNormalQuad( Ra::Core::Vector2( -1.f, 1.f ) );
        auto qm = std::make_unique<Ra::Engine::Data::Mesh>( "caller" );
        qm->loadGeometry( std::move( mesh ) );
        m_quadMesh = std::move( qm );
        m_quadMesh->updateGL();

        const std::string composeVertexShader { "layout (location = 0) in vec3 in_position;\n"
                                                "out vec2 varTexcoord;\n"
                                                "void main()\n"
                                                "{\n"
                                                "  gl_Position = vec4(in_position, 1.0);\n"
                                                "  varTexcoord = (in_position.xy + 1.0) * 0.5;\n"
                                                "}\n" };
        const std::string composeFragmentShader {
            "in vec2 varTexcoord;\n"
            "out vec4 f_Color;\n"
            "uniform sampler2D u_OITSumColor;\n"
            "uniform sampler2D u_OITSumWeight;\n"
            "void main() {\n"
            "   float r = texture( u_OITSumWeight, varTexcoord ).r;\n"
            "   if ( r >= 1.0 ) { discard; }\n"
            "   vec4 accum = texture( u_OITSumColor, varTexcoord );\n"
            "   vec3 avg_color = accum.rgb / max( accum.a, 0.00001 );\n"
            "   f_Color = vec4( avg_color, r );\n"
            "}" };

        Ra::Engine::Data::ShaderConfiguration config { "TransparencyNode::ComposeTransparency" };
        config.addShaderSource( Ra::Engine::Data::ShaderType::ShaderType_VERTEX,
                                composeVertexShader );
        config.addShaderSource( Ra::Engine::Data::ShaderType::ShaderType_FRAGMENT,
                                composeFragmentShader );
        if ( m_shader == nullptr ) {
            auto added = m_shaderMngr->addShaderProgram( config );
            if ( added ) {
                m_shader     = added.value();
                m_hasShaders = true;
            }
        }
    }
    return m_hasShaders;
}
void TransparentLocalLightingNode::destroy() {
    delete m_framebuffer;
    delete m_oitFramebuffer;
    delete m_accumulationTexture;
    delete m_revealageTexture;

    delete m_nodeState;
    delete m_composeState;
}

void TransparentLocalLightingNode::resize( uint32_t width, uint32_t height ) {
    m_accumulationTexture->resize( width, height );
    m_revealageTexture->resize( width, height );
    m_oitFramebuffer->bind();
    m_oitFramebuffer->attachTexture( gl::GL_COLOR_ATTACHMENT0, m_accumulationTexture->texture() );
    m_oitFramebuffer->attachTexture( gl::GL_COLOR_ATTACHMENT1, m_revealageTexture->texture() );
    m_oitFramebuffer->unbind();
}

void TransparentLocalLightingNode::buildRenderTechnique(
    const Ra::Engine::Rendering::RenderObject* ro,
    Ra::Engine::Rendering::RenderTechnique& rt ) const {

    auto mat = const_cast<Ra::Engine::Rendering::RenderObject*>( ro )->getMaterial();
    // Volumes are not used in EnvLightPass
    if ( mat->getMaterialAspect() == Ra::Engine::Data::Material::MaterialAspect::MAT_DENSITY ) {
        return;
    }

    if ( auto cfg = Ra::Engine::Data::ShaderConfigurationFactory::getConfiguration(
             { "TransparencyLocalLightNode::" + mat->getMaterialName() } ) ) {
        rt.setConfiguration( *cfg, m_idx );
    }
    else {
        std::string resourcesRootDir = m_resourceDir + "Shaders/TransparencyLocalLightNode/";
        // Build the shader configuration
        Ra::Engine::Data::ShaderConfiguration theConfig {
            { "TransparencyLocalLightNode::" + mat->getMaterialName() },
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

bool TransparentLocalLightingNode::execute() {
    // Get parameters
    // Render objects
    auto& renderObjects = m_inObjects->getData();
    // Cameras
    auto& camera = m_inCamera->getData();
    // Lights
    auto& lights = m_inLights->getData();

    // Color tex
    m_colorTexture = &m_inColor->getData();
    m_outColor->setData( m_colorTexture );

    // Depth buffer
    auto& depthBuffer = m_inDepth->getData();

    auto currentState = globjects::State::currentState();
    m_nodeState->apply();
    Ra::Engine::Data::RenderParameters inPassParams;

    // Render transparent objects
    static const float clearZeros[4] = { 0.0, 0.0, 0.0, 1.0 };
    static const float clearOnes[4]  = { 1.0, 1.0, 1.0, 1.0 };
    const gl::GLenum buffers[]       = { gl::GL_COLOR_ATTACHMENT0, gl::GL_COLOR_ATTACHMENT1 };
    m_oitFramebuffer->bind();
    m_oitFramebuffer->attachTexture( gl::GL_DEPTH_ATTACHMENT, depthBuffer.texture() );
    gl::glDrawBuffers( 2, buffers );
    gl::glClearBufferfv( gl::GL_COLOR, 0, clearZeros );
    gl::glClearBufferfv( gl::GL_COLOR, 1, clearOnes );
    gl::glBlendEquation( gl::GL_FUNC_ADD );
    gl::glBlendFunci( 0, gl::GL_ONE, gl::GL_ONE );
    gl::glBlendFunci( 1, gl::GL_ZERO, gl::GL_ONE_MINUS_SRC_ALPHA );
    if ( lights.size() > 0 ) {
        for ( const auto& l : lights ) {
            l->getRenderParameters( inPassParams );
            for ( const auto& ro : renderObjects ) {
                ro->render( inPassParams, camera, m_idx );
            }
        }
    }
    m_oitFramebuffer->detach( gl::GL_DEPTH_ATTACHMENT );

    // Compute the result
    m_framebuffer->bind();
    m_framebuffer->attachTexture( gl::GL_DEPTH_ATTACHMENT, depthBuffer.texture() );
    m_framebuffer->attachTexture( gl::GL_COLOR_ATTACHMENT0, m_colorTexture->texture() );
    gl::glDrawBuffers( 1, buffers );
    m_composeState->apply();

    m_shader->bind();
    m_shader->setUniform( "u_OITSumColor", m_accumulationTexture, 0 );
    m_shader->setUniform( "u_OITSumWeight", m_revealageTexture, 1 );
    m_quadMesh->render( m_shader );

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
