#include <Dataflow/Rendering/Nodes/RenderNodes/VolumeLocalLightingNode.hpp>

#include <Engine/Data/Material.hpp>
#include <Engine/Data/Mesh.hpp>
#include <Engine/Data/ShaderConfigFactory.hpp>
#include <Engine/Data/ShaderProgramManager.hpp>

namespace Ra {
namespace Dataflow {
namespace Rendering {
namespace Nodes {

using namespace Ra::Engine::Data;

VolumeLocalLightingNode::VolumeLocalLightingNode( const std::string& name ) :
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
}

void VolumeLocalLightingNode::init() {
    m_framebuffer       = new globjects::Framebuffer();
    m_volumeFramebuffer = new globjects::Framebuffer();

    Ra::Engine::Data::TextureParameters texParams;
    texParams.target         = gl::GL_TEXTURE_2D;
    texParams.minFilter      = gl::GL_LINEAR;
    texParams.magFilter      = gl::GL_LINEAR;
    texParams.internalFormat = gl::GL_RGBA32F;
    texParams.format         = gl::GL_RGBA;
    texParams.type           = gl::GL_FLOAT;
    texParams.name           = "Volume";
    m_volumeTexture          = new Ra::Engine::Data::Texture( texParams );

    m_nodeState = new globjects::State( globjects::State::DeferredMode );
    m_nodeState->enable( gl::GL_DEPTH_TEST );
    m_nodeState->depthFunc( gl::GL_LEQUAL );
    m_nodeState->depthMask( gl::GL_FALSE );
    m_nodeState->colorMask( gl::GL_TRUE, gl::GL_TRUE, gl::GL_TRUE, gl::GL_TRUE );
    m_nodeState->disable( gl::GL_BLEND );

    m_composeState = new globjects::State( globjects::State::DeferredMode );
    m_composeState->disable( gl::GL_DEPTH_TEST );
    m_composeState->depthMask( gl::GL_FALSE );
    m_composeState->colorMask( gl::GL_TRUE, gl::GL_TRUE, gl::GL_TRUE, gl::GL_TRUE );
    m_composeState->blendFunc( gl::GL_ONE_MINUS_SRC_ALPHA, gl::GL_SRC_ALPHA );
    m_composeState->enable( gl::GL_BLEND );
}

bool VolumeLocalLightingNode::initInternalShaders() {
    if ( m_shader == nullptr ) {
        Ra::Core::Geometry::TriangleMesh mesh =
            Ra::Core::Geometry::makeZNormalQuad( Ra::Core::Vector2( -1.f, 1.f ) );
        auto qm = std::make_unique<Ra::Engine::Data::Mesh>( "caller" );
        qm->loadGeometry( std::move( mesh ) );
        m_quadMesh = std::move( qm );
        m_quadMesh->updateGL();

        const std::string vrtxSrc { "layout (location = 0) in vec3 in_position;\n"
                                    "out vec2 varTexcoord;\n"
                                    "void main()\n"
                                    "{\n"
                                    "  gl_Position = vec4(in_position, 1.0);\n"
                                    "  varTexcoord = (in_position.xy + 1.0) / 2.0;\n"
                                    "}\n" };
        const std::string frgSrc {
            "out vec4 fragColor;\n"
            "in vec2 varTexcoord;\n"
            "uniform sampler2D volumeImage;\n"
            "void main()\n"
            "{\n"
            "  vec2 size = vec2(textureSize(volumeImage, 0));\n"
            "  vec4 volColor = texelFetch(volumeImage, ivec2(varTexcoord.xy * size), 0);\n"
            "  if (volColor.a < 1)\n"
            "    discard;\n"
            "  fragColor = vec4(volColor.rgb, 0);\n"
            "}\n" };
        Ra::Engine::Data::ShaderConfiguration config { "ComposeVolume" };
        config.addShaderSource( Ra::Engine::Data::ShaderType::ShaderType_VERTEX, vrtxSrc );
        config.addShaderSource( Ra::Engine::Data::ShaderType::ShaderType_FRAGMENT, frgSrc );
        auto added = m_shaderMngr->addShaderProgram( config );
        if ( added ) {
            m_shader     = added.value();
            m_hasShaders = true;
        }
    }
    return m_shader != nullptr;
}
void VolumeLocalLightingNode::destroy() {
    delete m_framebuffer;
    delete m_volumeFramebuffer;
    delete m_volumeTexture;

    delete m_nodeState;
    delete m_composeState;
}

void VolumeLocalLightingNode::resize( uint32_t width, uint32_t height ) {
    m_volumeTexture->resize( width, height );

    m_volumeFramebuffer->bind();
    m_volumeFramebuffer->attachTexture( gl::GL_COLOR_ATTACHMENT0, m_volumeTexture->texture() );
    m_volumeFramebuffer->unbind();
}

void VolumeLocalLightingNode::buildRenderTechnique(
    const Ra::Engine::Rendering::RenderObject* ro,
    Ra::Engine::Rendering::RenderTechnique& rt ) const {
    auto mat = const_cast<Ra::Engine::Rendering::RenderObject*>( ro )->getMaterial();
    // Only volumes are used by this pass
    if ( mat->getMaterialAspect() != Ra::Engine::Data::Material::MaterialAspect::MAT_DENSITY ) {
        return;
    }
    // use the standard Radium shaders
    auto passconfig =
        Ra::Engine::Data::ShaderConfigurationFactory::getConfiguration( "Volumetric" );
    rt.setConfiguration( *passconfig, m_idx );
    rt.setParametersProvider( mat, m_idx );
}

bool VolumeLocalLightingNode::execute() {
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

    // Render transparent objects
    static const float clearZeros[4] = { 0.0, 0.0, 0.0, 0.0 };
    const gl::GLenum buffers[]       = { gl::GL_COLOR_ATTACHMENT0 };

    m_volumeFramebuffer->bind();
    m_nodeState->apply();
    m_volumeFramebuffer->attachTexture( gl::GL_DEPTH_ATTACHMENT, depthBuffer.texture() );

    gl::glDrawBuffers( 1, buffers );
    gl::glClearBufferfv( gl::GL_COLOR, 0, clearZeros );
    Ra::Engine::Data::RenderParameters inPassParams;
    inPassParams.addParameter( "imageColor", m_colorTexture );
    inPassParams.addParameter( "imageDepth", depthBuffer.texture() );
    if ( lights.size() > 0 ) {
        for ( const auto& l : lights ) {
            l->getRenderParameters( inPassParams );
            for ( const auto& ro : renderObjects ) {
                ro->render( inPassParams, camera, m_idx );
            }
        }
    }
    // m_volumeFramebuffer->detach( gl::GL_DEPTH_ATTACHMENT );

    // Compute the result
    m_framebuffer->bind();
    m_framebuffer->attachTexture( gl::GL_COLOR_ATTACHMENT0, m_colorTexture->texture() );
    gl::glDrawBuffers( 1, buffers );
    m_composeState->apply();

    m_shader->bind();
    m_shader->setUniform( "volumeImage", m_volumeTexture, 0 );
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
