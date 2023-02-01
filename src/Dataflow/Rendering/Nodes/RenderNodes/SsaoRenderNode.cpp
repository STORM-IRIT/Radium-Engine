#include <Dataflow/Rendering/Nodes/RenderNodes/SsaoRenderNode.hpp>

#include <Core/Geometry/MeshPrimitives.hpp>
#include <Engine/Data/Mesh.hpp>
#include <Engine/Data/RenderParameters.hpp>
#include <Engine/Data/ShaderConfiguration.hpp>
#include <Engine/Data/ShaderProgramManager.hpp>
#include <Engine/RadiumEngine.hpp>

namespace Ra {
namespace Dataflow {
namespace Rendering {
namespace Nodes {

using namespace Ra::Engine::Data;

SsaoNode::SsaoNode( const std::string& name ) : RenderingNode( name, getTypename() ) {
    addInput( m_inWorldPos );
    m_inWorldPos->mustBeLinked();

    addInput( m_inWorldNormal );
    m_inWorldNormal->mustBeLinked();

    addInput( m_inCamera );
    m_inCamera->mustBeLinked();

    addInput( m_aoRadius );
    addInput( m_aoSamples );

    Ra::Engine::Data::TextureParameters texParams;
    texParams.target         = gl::GL_TEXTURE_2D;
    texParams.minFilter      = gl::GL_LINEAR;
    texParams.magFilter      = gl::GL_LINEAR;
    texParams.internalFormat = gl::GL_RGBA32F;
    texParams.format         = gl::GL_RGBA;
    texParams.type           = gl::GL_FLOAT;
    texParams.name           = "ssao";
    m_AO                     = new Ra::Engine::Data::Texture( texParams );

    addOutput( m_ssao, m_AO );

    auto editableRadius = new EditableParameter( "radius", m_editableAORadius );
    editableRadius->addAdditionalData( 0. );
    editableRadius->addAdditionalData( 100. );
    addEditableParameter( editableRadius );

    auto editableSamples = new EditableParameter( "samples", m_editableSamples );
    editableSamples->addAdditionalData( 0 );
    editableSamples->addAdditionalData( 4096 );
    addEditableParameter( editableSamples );
}

void SsaoNode::init() {
    Ra::Engine::Data::TextureParameters texParams;
    texParams.target         = gl::GL_TEXTURE_2D;
    texParams.minFilter      = gl::GL_LINEAR;
    texParams.magFilter      = gl::GL_LINEAR;
    texParams.internalFormat = gl::GL_RGBA32F;
    texParams.format         = gl::GL_RGBA;
    texParams.type           = gl::GL_FLOAT;
    texParams.name           = "Raw Ambient Occlusion";
    m_rawAO                  = new Ra::Engine::Data::Texture( texParams );

    Ra::Core::Geometry::TriangleMesh mesh =
        Ra::Core::Geometry::makeZNormalQuad( Ra::Core::Vector2( -1.f, 1.f ) );
    auto qm = std::make_unique<Ra::Engine::Data::Mesh>( "caller" );
    qm->loadGeometry( std::move( mesh ) );
    m_quadMesh = std::move( qm );
    m_quadMesh->updateGL();

    // TODO make the sampling method an editable parameter
    m_sphereSampler =
        std::make_unique<SphereSampler>( SphereSampler::SamplingMethod::HAMMERSLEY, 64 );

    m_blurFramebuffer = new globjects::Framebuffer();
    m_framebuffer     = new globjects::Framebuffer();
}

void SsaoNode::destroy() {
    delete m_rawAO;
    delete m_AO;
    delete m_blurFramebuffer;
    delete m_framebuffer;
}

void SsaoNode::resize( uint32_t width, uint32_t height ) {
    m_rawAO->resize( width, height );
    m_AO->resize( width, height );

    m_framebuffer->bind();
    m_framebuffer->attachTexture( gl::GL_COLOR_ATTACHMENT0, m_rawAO->texture() );

    m_blurFramebuffer->bind();
    m_blurFramebuffer->attachTexture( gl::GL_COLOR_ATTACHMENT0, m_AO->texture() );

    globjects::Framebuffer::unbind();
}

bool SsaoNode::execute() {
    auto aabb = Ra::Engine::RadiumEngine::getInstance()->computeSceneAabb();
    if ( aabb.isEmpty() ) { m_sceneDiag = 1_ra; }
    else {
        m_sceneDiag = aabb.diagonal().norm();
    }

    Ra::Engine::Data::RenderParameters inPassParams;
    // Positions
    auto posTexture = &m_inWorldPos->getData();
    // Normals
    auto normalTexture = &m_inWorldNormal->getData();

    // AO Radius
    auto aoRadius = m_editableAORadius;
    if ( m_aoRadius->isLinked() ) { aoRadius = m_aoRadius->getData(); }

    // AO Samples
    auto samples = m_editableSamples;
    if ( m_aoSamples->isLinked() ) { samples = m_aoSamples->getData(); }
    if ( m_currentSamples != samples ) {
        m_currentSamples = samples;
        m_sphereSampler  = std::make_unique<SphereSampler>(
            SphereSampler::SamplingMethod::HAMMERSLEY, m_currentSamples );
    }

    // Cameras
    auto& camera = m_inCamera->getData();

    m_framebuffer->bind();
    const gl::GLenum buffers[] = { gl::GL_COLOR_ATTACHMENT0 };
    gl::glDrawBuffers( 1, buffers );
    float clearWhite[4] = { 1.0f, 1.0f, 1.0f, 0.0f };
    gl::glClearBufferfv( gl::GL_COLOR, 0, clearWhite );
    gl::glDisable( gl::GL_DEPTH_TEST );
    gl::glDepthMask( gl::GL_FALSE );

    m_shader->bind();
    Ra::Core::Matrix4 viewProj = camera.projMatrix * camera.viewMatrix;

    m_shader->setUniform( "transform.mvp", viewProj );
    m_shader->setUniform( "transform.proj", camera.projMatrix );
    m_shader->setUniform( "transform.view", camera.viewMatrix );

    m_shader->setUniform( "normal_sampler", normalTexture, 0 );
    m_shader->setUniform( "position_sampler", posTexture, 1 );
    m_shader->setUniform( "dir_sampler", m_sphereSampler->asTexture(), 2 );
    m_shader->setUniform( "ssdoRadius", aoRadius / 100_ra * m_sceneDiag );

    m_quadMesh->render( m_shader );
    gl::glEnable( gl::GL_DEPTH_TEST );
    gl::glDepthMask( gl::GL_TRUE );
    m_framebuffer->unbind();

    m_blurFramebuffer->bind();
    m_blurShader->bind();
    gl::glDrawBuffers( 1, buffers );
    gl::glClearBufferfv( gl::GL_COLOR, 0, clearWhite );
    gl::glDisable( gl::GL_DEPTH_TEST );
    gl::glDepthMask( gl::GL_FALSE );

    m_shader->setUniform( "transform.mvp", viewProj );
    m_shader->setUniform( "transform.proj", camera.projMatrix );
    m_shader->setUniform( "transform.view", camera.viewMatrix );
    m_shader->setUniform( "ao_sampler", m_rawAO, 0 );

    m_quadMesh->render( m_shader );
    gl::glEnable( gl::GL_DEPTH_TEST );
    gl::glDepthMask( gl::GL_TRUE );
    m_blurFramebuffer->unbind();

    return true;
}

void SsaoNode::toJsonInternal( nlohmann::json& data ) const {
    if ( m_currentSamples != AO_DefaultSamples ) {
        // do not write default value
        data["samples"] = m_currentSamples;
    }
    if ( m_editableAORadius != AO_DefaultRadius ) {
        // do not write default value
        data["radius"] = m_editableAORadius;
    }
}

bool SsaoNode::fromJsonInternal( const nlohmann::json& data ) {
    if ( data.contains( "radius" ) ) { m_editableAORadius = data["radius"]; }
    if ( data.contains( "samples" ) ) {
        m_currentSamples  = data["samples"];
        m_editableSamples = Scalar( m_currentSamples );
    }
    return true;
}

bool SsaoNode::initInternalShaders() {
    if ( !m_hasShaders ) {
        const std::string vertexShaderSource { "layout (location = 0) in vec3 in_position;\n"
                                               "out vec2 varTexcoord;\n"
                                               "void main(void) {\n"
                                               "    gl_Position = vec4(in_position.xyz, 1.0);\n"
                                               "    varTexcoord = (in_position.xy + 1.0) / 2.0;\n"
                                               "}" };
        std::string resourcesRootDir = m_resourceDir + "Shaders/SsaoNode/";

        Ra::Engine::Data::ShaderConfiguration ssdoConfig { "SSDO" };
        ssdoConfig.addShaderSource( Ra::Engine::Data::ShaderType::ShaderType_VERTEX,
                                    vertexShaderSource );
        ssdoConfig.addShader( Ra::Engine::Data::ShaderType::ShaderType_FRAGMENT,
                              resourcesRootDir + "ssao.frag.glsl" );
        auto added = m_shaderMngr->addShaderProgram( ssdoConfig );
        if ( added ) { m_shader = added.value(); }
        else {
            std::cout << "AO: Could not add shader." << std::endl;
            return false;
        }

        Ra::Engine::Data::ShaderConfiguration blurssdoConfig { "blurSSDO" };
        blurssdoConfig.addShaderSource( Ra::Engine::Data::ShaderType::ShaderType_VERTEX,
                                        vertexShaderSource );
        blurssdoConfig.addShader( Ra::Engine::Data::ShaderType::ShaderType_FRAGMENT,
                                  resourcesRootDir + "blurao.frag.glsl" );
        added = m_shaderMngr->addShaderProgram( blurssdoConfig );
        if ( added ) { m_blurShader = added.value(); }
        else {
            std::cout << "AO: Could not add shader." << std::endl;
            return false;
        }
        m_hasShaders = true;
    }
    return m_hasShaders;
}

} // namespace Nodes
} // namespace Rendering
} // namespace Dataflow
} // namespace Ra
