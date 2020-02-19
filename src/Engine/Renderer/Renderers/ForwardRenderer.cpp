#include <Engine/Renderer/Renderers/ForwardRenderer.hpp>

#include <Core/Containers/MakeShared.hpp>
#include <Core/Resources/Resources.hpp>
#include <Core/Utils/Color.hpp>
#include <Core/Utils/Log.hpp>

#include <Engine/Managers/CameraManager/DefaultCameraManager.hpp>
#include <Engine/Managers/LightManager/DefaultLightManager.hpp>
#include <Engine/Renderer/Light/Light.hpp>
#include <Engine/Renderer/Material/Material.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/OpenGL/OpenGL.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderTechnique/RenderParameters.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>
#include <Engine/Renderer/Renderers/DebugRender.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>
#include <globjects/Framebuffer.h>

namespace Ra {
namespace Engine {

using namespace Core::Utils; // log

namespace {
const GLenum buffers[] = {GL_COLOR_ATTACHMENT0,
                          GL_COLOR_ATTACHMENT1,
                          GL_COLOR_ATTACHMENT2,
                          GL_COLOR_ATTACHMENT3,
                          GL_COLOR_ATTACHMENT4,
                          GL_COLOR_ATTACHMENT5,
                          GL_COLOR_ATTACHMENT6,
                          GL_COLOR_ATTACHMENT7};
}

ForwardRenderer::ForwardRenderer() : Renderer() {}

ForwardRenderer::~ForwardRenderer() {}

void ForwardRenderer::initializeInternal() {
    initShaders();
    initBuffers();

    auto cameraManager = new DefaultCameraManager();
    Ra::Engine::RadiumEngine::getInstance()->registerSystem( "DefaultCameraManager",
                                                             cameraManager );

    auto lightManager = new DefaultLightManager();
    Ra::Engine::RadiumEngine::getInstance()->registerSystem( "DefaultLightManager", lightManager );
    m_lightmanagers.push_back( lightManager );

    if ( !DebugRender::getInstance() )
    {
        DebugRender::createInstance();
        DebugRender::getInstance()->initialize();
    }
}

void ForwardRenderer::initShaders() {
    /// For internal resources management in a filesystem
    std::string resourcesRootDir = {Core::Resources::getRadiumResourcesDir()};
    m_shaderMgr->addShaderProgram( {{"Hdr2Ldr"},
                                    resourcesRootDir + "Shaders/2DShaders/Basic2D.vert.glsl",
                                    resourcesRootDir + "Shaders/2DShaders/Hdr2Ldr.frag.glsl"} );
    m_shaderMgr->addShaderProgram( {{"ComposeOIT"},
                                    resourcesRootDir + "Shaders/2DShaders/Basic2D.vert.glsl",
                                    resourcesRootDir + "Shaders/2DShaders/ComposeOIT.frag.glsl"} );
}

void ForwardRenderer::initBuffers() {
    m_fbo            = std::make_unique<globjects::Framebuffer>();
    m_oitFbo         = std::make_unique<globjects::Framebuffer>();
    m_postprocessFbo = std::make_unique<globjects::Framebuffer>();
    m_uiXrayFbo      = std::make_unique<globjects::Framebuffer>();
    m_volumeFbo      = std::make_unique<globjects::Framebuffer>();
    // Forward renderer internal textures texture

    TextureParameters texparams;
    texparams.width  = m_width;
    texparams.height = m_height;
    texparams.target = GL_TEXTURE_2D;

    // Depth texture
    texparams.minFilter                = GL_NEAREST;
    texparams.magFilter                = GL_NEAREST;
    texparams.internalFormat           = GL_DEPTH_COMPONENT24;
    texparams.format                   = GL_DEPTH_COMPONENT;
    texparams.type                     = GL_UNSIGNED_INT;
    texparams.name                     = "Depth (fw renderer)";
    m_textures[RendererTextures_Depth] = std::make_unique<Texture>( texparams );

    // Color texture
    texparams.internalFormat = GL_RGBA32F;
    texparams.format         = GL_RGBA;
    texparams.type           = GL_FLOAT;
    texparams.minFilter      = GL_LINEAR;
    texparams.magFilter      = GL_LINEAR;

    texparams.name                   = "HDR";
    m_textures[RendererTextures_HDR] = std::make_unique<Texture>( texparams );

    texparams.name                      = "Normal";
    m_textures[RendererTextures_Normal] = std::make_unique<Texture>( texparams );

    texparams.name                       = "Diffuse";
    m_textures[RendererTextures_Diffuse] = std::make_unique<Texture>( texparams );

    texparams.name                        = "Specular";
    m_textures[RendererTextures_Specular] = std::make_unique<Texture>( texparams );

    texparams.name                        = "OIT Accum";
    m_textures[RendererTextures_OITAccum] = std::make_unique<Texture>( texparams );

    texparams.name                            = "OIT Revealage";
    m_textures[RendererTextures_OITRevealage] = std::make_unique<Texture>( texparams );

    texparams.name                      = "Volume";
    m_textures[RendererTextures_Volume] = std::make_unique<Texture>( texparams );

    m_secondaryTextures["Depth (fw)"]       = m_textures[RendererTextures_Depth].get();
    m_secondaryTextures["HDR Texture"]      = m_textures[RendererTextures_HDR].get();
    m_secondaryTextures["Normal Texture"]   = m_textures[RendererTextures_Normal].get();
    m_secondaryTextures["Diffuse Texture"]  = m_textures[RendererTextures_Diffuse].get();
    m_secondaryTextures["Specular Texture"] = m_textures[RendererTextures_Specular].get();
    m_secondaryTextures["OIT Accum"]        = m_textures[RendererTextures_OITAccum].get();
    m_secondaryTextures["OIT Revealage"]    = m_textures[RendererTextures_OITRevealage].get();

    // Volume texture is not exposed ...
    m_secondaryTextures["Volume"] = m_textures[RendererTextures_Volume].get();
}

void ForwardRenderer::updateStepInternal( const ViewingParameters& renderData ) {
    CORE_UNUSED( renderData );

    m_transparentRenderObjects.clear();
    m_volumetricRenderObjects.clear();
    for ( auto it = m_fancyRenderObjects.begin(); it != m_fancyRenderObjects.end(); )
    {
        if ( ( *it )->isTransparent() )
        {
            m_transparentRenderObjects.push_back( *it );
            it = m_fancyRenderObjects.erase( it );
        }
        else
        {
            auto material = ( *it )->getMaterial();
            if ( material &&
                 material->getMaterialAspect() == Material::MaterialAspect::MAT_DENSITY )
            {
                m_volumetricRenderObjects.push_back( *it );
                it = m_fancyRenderObjects.erase( it );
            }
            else
            { ++it; }
        }
    }
    m_fancyTransparentCount = m_transparentRenderObjects.size();
    m_fancyVolumetricCount  = m_volumetricRenderObjects.size();
}

void ForwardRenderer::renderInternal( const ViewingParameters& renderData ) {

    m_fbo->bind();

    GL_ASSERT( glEnable( GL_DEPTH_TEST ) );
    GL_ASSERT( glDepthMask( GL_TRUE ) );
    GL_ASSERT( glColorMask( 1, 1, 1, 1 ) );

    GL_ASSERT( glDrawBuffers( 4, buffers ) );

    static const auto clearZeros = Core::Utils::Color::Black();
    static const auto clearOnes  = Core::Utils::Color::White();
    static const float clearDepth{1.0f};

    GL_ASSERT( glClearBufferfv( GL_COLOR, 0, getBackgroundColor().data() ) ); // Clear color
    GL_ASSERT( glClearBufferfv( GL_COLOR, 1, clearZeros.data() ) );           // Clear normals
    GL_ASSERT( glClearBufferfv( GL_COLOR, 2, clearZeros.data() ) );           // Clear diffuse
    GL_ASSERT( glClearBufferfv( GL_COLOR, 3, clearZeros.data() ) );           // Clear specular
    GL_ASSERT( glClearBufferfv( GL_DEPTH, 0, &clearDepth ) );                 // Clear depth

    // Z prepass
    GL_ASSERT( glDepthFunc( GL_LESS ) );
    GL_ASSERT( glDisable( GL_BLEND ) );
    GL_ASSERT( glPointSize( 3.f ) );

    // Set in RenderParam the configuration about ambiant lighting (instead of hard constant
    // direclty in shaders)
    RenderParameters zprepassParams;
    for ( const auto& ro : m_fancyRenderObjects )
    {
        ro->render( zprepassParams, renderData, DefaultRenderingPasses::Z_PREPASS );
    }
    // Transparent objects are rendered in the Z-prepass, but only their fully opaque fragments (if
    // any) might influence the z-buffer Rendering transparent objects assuming that they discard
    // all their non-opaque fragments
    for ( const auto& ro : m_transparentRenderObjects )
    {
        ro->render( zprepassParams, renderData, DefaultRenderingPasses::Z_PREPASS );
    }
    // Volumetric objects are not rendered in the Z-prepass

    // Opaque Lighting pass
    GL_ASSERT( glDepthFunc( GL_LEQUAL ) );
    GL_ASSERT( glDepthMask( GL_FALSE ) );

    GL_ASSERT( glEnable( GL_BLEND ) );
    GL_ASSERT( glBlendFunc( GL_ONE, GL_ONE ) );

    GL_ASSERT( glDrawBuffers( 1, buffers ) ); // Draw color texture

    // Radium V2 : this render loop might be greatly improved by inverting light and objects loop
    // Make shaders bounded only once, minimize full stats-changes, ...
    if ( m_lightmanagers[0]->count() > 0 )
    {
        // for ( const auto& l : m_lights )
        for ( size_t i = 0; i < m_lightmanagers[0]->count(); ++i )
        {
            const auto l = m_lightmanagers[0]->getLight( i );
            RenderParameters lightingpassParams;
            l->getRenderParameters( lightingpassParams );

            for ( const auto& ro : m_fancyRenderObjects )
            {
                ro->render(
                    lightingpassParams, renderData, DefaultRenderingPasses::LIGHTING_OPAQUE );
            }
            // Rendering transparent objects assuming that they discard all their non-opaque
            // fragments
            for ( const auto& ro : m_transparentRenderObjects )
            {
                ro->render(
                    lightingpassParams, renderData, DefaultRenderingPasses::LIGHTING_OPAQUE );
            }
        }
    }
    else
    { LOG( logINFO ) << "Opaque : no light sources, unable to render"; }

    // Transparency (blending) pass
    if ( !m_transparentRenderObjects.empty() )
    {
        m_fbo->unbind();

        m_oitFbo->bind();

        GL_ASSERT( glDrawBuffers( 2, buffers ) );
        GL_ASSERT( glClearBufferfv( GL_COLOR, 0, clearZeros.data() ) );
        GL_ASSERT( glClearBufferfv( GL_COLOR, 1, clearOnes.data() ) );

        GL_ASSERT( glDepthFunc( GL_LESS ) );
        GL_ASSERT( glEnable( GL_BLEND ) );

        GL_ASSERT( glBlendEquation( GL_FUNC_ADD ) );
        GL_ASSERT( glBlendFunci( 0, GL_ONE, GL_ONE ) );
        GL_ASSERT( glBlendFunci( 1, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA ) );

        if ( m_lightmanagers[0]->count() > 0 )
        {
            // for ( const auto& l : m_lights )
            for ( size_t i = 0; i < m_lightmanagers[0]->count(); ++i )
            {
                const auto l = m_lightmanagers[0]->getLight( i );
                RenderParameters trasparencypassParams;
                l->getRenderParameters( trasparencypassParams );

                for ( const auto& ro : m_transparentRenderObjects )
                {
                    ro->render( trasparencypassParams,
                                renderData,
                                DefaultRenderingPasses::LIGHTING_TRANSPARENT );
                }
            }
        }
        else
        { LOG( logINFO ) << "Transparent : no light sources, unable to render"; }

        m_oitFbo->unbind();

        m_fbo->bind();
        GL_ASSERT( glDrawBuffers( 1, buffers ) );
        GL_ASSERT( glDisable( GL_DEPTH_TEST ) );
        GL_ASSERT( glBlendFunc( GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA ) );
        {
            auto shader = m_shaderMgr->getShaderProgram( "ComposeOIT" );
            shader->bind();
            shader->setUniform( "u_OITSumColor", m_textures[RendererTextures_OITAccum].get(), 0 );
            shader->setUniform(
                "u_OITSumWeight", m_textures[RendererTextures_OITRevealage].get(), 1 );

            m_quadMesh->render( shader );
        }
        GL_ASSERT( glEnable( GL_DEPTH_TEST ) );
    }
    // Volumetric pass
    // Z-test is enabled but z-write must be disable to allow access to the z-buffer in the shader.
    // This pass render in its own FBO and copy the result to the main colortexture
    if ( !m_volumetricRenderObjects.empty() )
    {
        m_volumeFbo->bind();
        GL_ASSERT( glDrawBuffers( 1, buffers ) );
        GL_ASSERT( glClearBufferfv( GL_COLOR, 0, Core::Utils::Color::Alpha().data() ) );
        GL_ASSERT( glDisable( GL_BLEND ) );
        // for ( const auto& l : m_lights )
        for ( size_t i = 0; i < m_lightmanagers[0]->count(); ++i )
        {
            const auto l = m_lightmanagers[0]->getLight( i );
            RenderParameters passParams;
            l->getRenderParameters( passParams );
            passParams.addParameter( "imageColor", m_textures[RendererTextures_HDR].get() );
            passParams.addParameter( "imageDepth", m_textures[RendererTextures_Depth].get() );

            for ( const auto& ro : m_volumetricRenderObjects )
            {
                ro->render( passParams, renderData, DefaultRenderingPasses::LIGHTING_VOLUMETRIC );
            }
        }
        m_volumeFbo->unbind();

        m_fbo->bind();
        GL_ASSERT( glDrawBuffers( 1, buffers ) );
        GL_ASSERT( glDisable( GL_DEPTH_TEST ) );
        GL_ASSERT( glEnable( GL_BLEND ) );
        GL_ASSERT( glBlendFunc( GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA ) );
        {
            auto shader = m_shaderMgr->getShaderProgram( "ComposeVolume" );
            shader->bind();
            shader->setUniform( "volumeImage", m_textures[RendererTextures_Volume].get(), 0 );
            m_quadMesh->render( shader );
        }
        GL_ASSERT( glEnable( GL_DEPTH_TEST ) );
    }
    // TODO : verify if this must be done before or after volumetric pass
    if ( m_wireframe )
    {
        //        m_fbo->bind();

        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        glEnable( GL_LINE_SMOOTH );
        glLineWidth( 1.f );
        glEnable( GL_POLYGON_OFFSET_LINE );
        glPolygonOffset( -1.0f, -1.1f );

        // Light pass
        GL_ASSERT( glDepthFunc( GL_LEQUAL ) );
        GL_ASSERT( glEnable( GL_BLEND ) );
        GL_ASSERT( glBlendFunc( GL_ONE, GL_ONE ) );

        GL_ASSERT( glDrawBuffers( 1, buffers ) ); // Draw color texture

        if ( m_lightmanagers[0]->count() > 0 )
        {
            // for ( const auto& l : m_lights )
            for ( size_t i = 0; i < m_lightmanagers[0]->count(); ++i )
            {
                const auto l = m_lightmanagers[0]->getLight( i );
                RenderParameters wireframepassParams;
                l->getRenderParameters( wireframepassParams );

                for ( const auto& ro : m_fancyRenderObjects )
                {
                    ro->render(
                        wireframepassParams, renderData, DefaultRenderingPasses::LIGHTING_OPAQUE );
                }
                // This will not work for the moment . skipping wireframe rendering of transparent
                // objects
#if 0
                for ( const auto& ro : m_transparentRenderObjects)
                {
                    ro->render( wireframepassParams, viewingParameters, RenderTechnique::LIGHTING_OPAQUE );
                }
#endif
            }
        }
        else
        { LOG( logINFO ) << "Wireframe : no light sources, unable to render"; }

        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        glDisable( GL_POLYGON_OFFSET_LINE );
    }

    // Restore state
    GL_ASSERT( glDepthFunc( GL_LESS ) );
    GL_ASSERT( glDisable( GL_BLEND ) );
    m_fbo->unbind();
}

// Draw debug stuff, do not overwrite depth map but do depth testing
void ForwardRenderer::debugInternal( const ViewingParameters& renderData ) {
    if ( m_drawDebug )
    {
        m_postprocessFbo->bind();
        GL_ASSERT( glDisable( GL_BLEND ) );
        GL_ASSERT( glEnable( GL_DEPTH_TEST ) );
        GL_ASSERT( glDepthMask( GL_FALSE ) );
        GL_ASSERT( glDepthFunc( GL_LESS ) );

        glDrawBuffers( 1, buffers );

        for ( const auto& ro : m_debugRenderObjects )
        {
            ro->render( RenderParameters{}, renderData );
        }

        DebugRender::getInstance()->render( renderData.viewMatrix, renderData.projMatrix );

        m_postprocessFbo->unbind();

        m_uiXrayFbo->bind();
        // Draw X rayed objects always on top of normal objects
        GL_ASSERT( glDepthMask( GL_TRUE ) );
        GL_ASSERT( glClear( GL_DEPTH_BUFFER_BIT ) );
        RenderParameters xrayLightParams;
        xrayLightParams.addParameter( "light.color", Ra::Core::Utils::Color::Grey( 5.0 ) );
        xrayLightParams.addParameter( "light.type", Light::LightType::DIRECTIONAL );
        xrayLightParams.addParameter( "light.directional.direction", Core::Vector3( 0, -1, 0 ) );
        for ( const auto& ro : m_xrayRenderObjects )
        {
            if ( ro->isVisible() ) { ro->render( xrayLightParams, renderData ); }
        }
        m_uiXrayFbo->unbind();
    }
}

// Draw UI stuff, always drawn on top of everything else + clear ZMask
void ForwardRenderer::uiInternal( const ViewingParameters& renderData ) {

    m_uiXrayFbo->bind();
    glDrawBuffers( 1, buffers );
    // Enable z-test
    GL_ASSERT( glDepthMask( GL_TRUE ) );
    GL_ASSERT( glEnable( GL_DEPTH_TEST ) );
    GL_ASSERT( glDepthFunc( GL_LESS ) );
    GL_ASSERT( glClear( GL_DEPTH_BUFFER_BIT ) );
    for ( const auto& ro : m_uiRenderObjects )
    {
        if ( ro->isVisible() )
        {
            auto shader = ro->getRenderTechnique()->getShader();

            // bind data
            shader->bind();

            Core::Matrix4 M  = ro->getTransformAsMatrix();
            Core::Matrix4 MV = renderData.viewMatrix * M;
            Core::Vector3 V  = MV.block<3, 1>( 0, 3 );
            Scalar d         = V.norm();

            Core::Matrix4 S    = Core::Matrix4::Identity();
            S.coeffRef( 0, 0 ) = S.coeffRef( 1, 1 ) = S.coeffRef( 2, 2 ) = d;

            M = M * S;

            shader->setUniform( "transform.proj", renderData.projMatrix );
            shader->setUniform( "transform.view", renderData.viewMatrix );
            shader->setUniform( "transform.model", M );

            auto shaderParameter = ro->getRenderTechnique()->getParametersProvider();
            if ( shaderParameter != nullptr ) shaderParameter->getParameters().bind( shader );

            // render
            ro->getMesh()->render( shader );
        }
    }
    m_uiXrayFbo->unbind();
}

void ForwardRenderer::postProcessInternal( const ViewingParameters& renderData ) {
    CORE_UNUSED( renderData );

    m_postprocessFbo->bind();

    GL_ASSERT( glDrawBuffers( 1, buffers ) );

    GL_ASSERT( glDisable( GL_DEPTH_TEST ) );
    GL_ASSERT( glDepthMask( GL_FALSE ) );

    const ShaderProgram* shader = m_shaderMgr->getShaderProgram( "Hdr2Ldr" );
    shader->bind();
    shader->setUniform( "screenTexture", m_textures[RendererTextures_HDR].get(), 0 );
    m_quadMesh->render( shader );

    GL_ASSERT( glDepthMask( GL_TRUE ) );
    GL_ASSERT( glEnable( GL_DEPTH_TEST ) );

    m_postprocessFbo->unbind();
}

void ForwardRenderer::resizeInternal() {
    m_pingPongSize = std::pow( uint( 2 ), uint( std::log2( std::min( m_width, m_height ) ) ) );

    m_textures[RendererTextures_Depth]->resize( m_width, m_height );
    m_textures[RendererTextures_HDR]->resize( m_width, m_height );
    m_textures[RendererTextures_Normal]->resize( m_width, m_height );
    m_textures[RendererTextures_Diffuse]->resize( m_width, m_height );
    m_textures[RendererTextures_Specular]->resize( m_width, m_height );
    m_textures[RendererTextures_OITAccum]->resize( m_width, m_height );
    m_textures[RendererTextures_OITRevealage]->resize( m_width, m_height );
    m_textures[RendererTextures_Volume]->resize( m_width, m_height );

    m_fbo->bind();
    m_fbo->attachTexture( GL_DEPTH_ATTACHMENT, m_textures[RendererTextures_Depth]->texture() );
    m_fbo->attachTexture( GL_COLOR_ATTACHMENT0, m_textures[RendererTextures_HDR]->texture() );
    m_fbo->attachTexture( GL_COLOR_ATTACHMENT1, m_textures[RendererTextures_Normal]->texture() );
    m_fbo->attachTexture( GL_COLOR_ATTACHMENT2, m_textures[RendererTextures_Diffuse]->texture() );
    m_fbo->attachTexture( GL_COLOR_ATTACHMENT3, m_textures[RendererTextures_Specular]->texture() );
    if ( m_fbo->checkStatus() != GL_FRAMEBUFFER_COMPLETE )
    { LOG( logERROR ) << "FBO Error (ForwardRenderer::m_fbo): " << m_fbo->checkStatus(); }

    m_volumeFbo->bind();
    m_volumeFbo->attachTexture( GL_DEPTH_ATTACHMENT,
                                m_textures[RendererTextures_Depth]->texture() );
    m_volumeFbo->attachTexture( GL_COLOR_ATTACHMENT0,
                                m_textures[RendererTextures_Volume]->texture() );
    if ( m_volumeFbo->checkStatus() != GL_FRAMEBUFFER_COMPLETE )
    {
        LOG( logERROR ) << "FBO Error (ForwardRenderer::m_volumeFbo) : "
                        << m_volumeFbo->checkStatus();
    }

    m_oitFbo->bind();
    m_oitFbo->attachTexture( GL_DEPTH_ATTACHMENT, m_textures[RendererTextures_Depth]->texture() );
    m_oitFbo->attachTexture( GL_COLOR_ATTACHMENT0,
                             m_textures[RendererTextures_OITAccum]->texture() );
    m_oitFbo->attachTexture( GL_COLOR_ATTACHMENT1,
                             m_textures[RendererTextures_OITRevealage]->texture() );
    if ( m_oitFbo->checkStatus() != GL_FRAMEBUFFER_COMPLETE )
    { LOG( logERROR ) << "FBO Error (ForwardRenderer::m_oitFbo) : " << m_oitFbo->checkStatus(); }

    m_postprocessFbo->bind();
    m_postprocessFbo->attachTexture( GL_DEPTH_ATTACHMENT,
                                     m_textures[RendererTextures_Depth]->texture() );
    m_postprocessFbo->attachTexture( GL_COLOR_ATTACHMENT0, m_fancyTexture->texture() );
    if ( m_postprocessFbo->checkStatus() != GL_FRAMEBUFFER_COMPLETE )
    {
        LOG( logERROR ) << "FBO Error (ForwardRenderer::m_postprocessFbo) : "
                        << m_postprocessFbo->checkStatus();
    }

    // FIXED : when m_postprocessFbo use the RendererTextures_Depth, the depth buffer is erased and
    // is therefore useless for future computation. Do not use this post-process FBO to render
    // eveything else than the scene. Create several FBO with ther own configuration (uncomment
    // Renderer::m_depthTexture->texture() to see the difference.)
    m_uiXrayFbo->bind();
    m_uiXrayFbo->attachTexture( GL_DEPTH_ATTACHMENT, Renderer::m_depthTexture->texture() );
    m_uiXrayFbo->attachTexture( GL_COLOR_ATTACHMENT0, m_fancyTexture->texture() );
    if ( m_uiXrayFbo->checkStatus() != GL_FRAMEBUFFER_COMPLETE )
    {
        LOG( logERROR ) << "FBO Error (ForwardRenderer::m_uiXrayFbo) : "
                        << m_uiXrayFbo->checkStatus();
    }
    // finished with fbo, undbind to bind default
    globjects::Framebuffer::unbind();
}

/*
 * Build renderTechnique for Forward Renderer : this is the default in Radium, so create Default
 * Render Technique
 */
bool ForwardRenderer::buildRenderTechnique( RenderObject* ro ) const {
    auto material = ro->getMaterial();
    auto builder  = EngineRenderTechniques::getDefaultTechnique( material->getMaterialName() );
    auto rt       = Core::make_shared<RenderTechnique>();
    // define the technique for rendering this RenderObject (here, using the default from Material
    // name)
    builder.second( *rt, material->isTransparent() );
    // If renderObject is a point cloud,  add geometry shader for splatting
    auto RenderedGeometry = dynamic_cast<const Mesh*>( ro->getMesh().get() );
    if ( RenderedGeometry && RenderedGeometry->getNumFaces() == 0 )
    {
        auto addGeomShader = [&rt]( Core::Utils::Index pass ) {
            if ( rt->hasConfiguration( pass ) )
            {
                ShaderConfiguration config = rt->getConfiguration( pass );
                config.addShader( ShaderType_GEOMETRY,
                                  std::string( Core::Resources::getRadiumResourcesDir() ) +
                                      "Shaders/Points/PointCloud.geom.glsl" );
                rt->setConfiguration( config, pass );
            }
        };

        addGeomShader( DefaultRenderingPasses::LIGHTING_OPAQUE );
        addGeomShader( DefaultRenderingPasses::LIGHTING_TRANSPARENT );
        addGeomShader( DefaultRenderingPasses::Z_PREPASS );
    }
    // make the material the parameter provider for the technique
    rt->setParametersProvider( material );
    ro->setRenderTechnique( rt );
    return true;
}

void ForwardRenderer::updateShadowMaps() {
    // Radium V2 : implement shadow mapping
}

} // namespace Engine
} // namespace Ra
