#include <Engine/Renderer/Renderers/ForwardRenderer.hpp>

#include <Core/Log/Log.hpp>
#include <Core/Math/ColorPresets.hpp>

#include <Engine/Managers/CameraManager/DefaultCameraManager.hpp>
#include <Engine/Managers/LightManager/DefaultLightManager.hpp>
#include <Engine/Renderer/Light/DirLight.hpp>
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


//#define NO_TRANSPARENCY
namespace Ra {
namespace Engine {

namespace {
const GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
                          GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5,
                          GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7};
}

ForwardRenderer::ForwardRenderer() : Renderer() {}

ForwardRenderer::~ForwardRenderer() {
};

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

    m_shaderMgr->addShaderProgram( { {"Hdr2Ldr"}, {"Shaders/HdrToLdr/Hdr2Ldr.vert.glsl"},
                                     {"Shaders/HdrToLdr/Hdr2Ldr.frag.glsl"} } );
#ifndef NO_TRANSPARENCY
    m_shaderMgr->addShaderProgram( { {"ComposeOIT"}, {"Shaders/Basic2D.vert.glsl"},
                                     {"Shaders/ComposeOIT.frag.glsl"} } );
#endif
}

void ForwardRenderer::initBuffers() {
    m_fbo = std::make_unique<globjects::Framebuffer>();
    m_oitFbo = std::make_unique<globjects::Framebuffer>();
    m_postprocessFbo = std::make_unique<globjects::Framebuffer>();
    m_uiXrayFbo = std::make_unique<globjects::Framebuffer>();
    // Forward renderer internal textures texture


    TextureParameters texparams;
    texparams.width = m_width;
    texparams.height = m_height;
    texparams.target = GL_TEXTURE_2D;

    // Depth texture
    texparams.minFilter = GL_NEAREST;
    texparams.magFilter = GL_NEAREST;
    texparams.internalFormat = GL_DEPTH_COMPONENT24;
    texparams.format = GL_DEPTH_COMPONENT;
    texparams.type = GL_UNSIGNED_INT;
    texparams.name = "Depth (fw renderer)";
    m_textures[RendererTextures_Depth] = std::make_unique<Texture>( texparams );

    // Color texture
    texparams.internalFormat = GL_RGBA32F;
    texparams.format = GL_RGBA;
    texparams.type = GL_FLOAT;
    texparams.minFilter = GL_LINEAR;
    texparams.magFilter = GL_LINEAR;

    texparams.name = "HDR";
    m_textures[RendererTextures_HDR] = std::make_unique<Texture>( texparams );

    texparams.name = "Normal";
    m_textures[RendererTextures_Normal] = std::make_unique<Texture>( texparams );

    texparams.name = "Diffuse";
    m_textures[RendererTextures_Diffuse] = std::make_unique<Texture>( texparams );

    texparams.name = "Specular";
    m_textures[RendererTextures_Specular] = std::make_unique<Texture>( texparams );

    texparams.name = "OIT Accum";
    m_textures[RendererTextures_OITAccum] = std::make_unique<Texture>( texparams );

    texparams.name = "OIT Revealage";
    m_textures[RendererTextures_OITRevealage] = std::make_unique<Texture>( texparams );


    m_secondaryTextures["Depth (fw)"] = m_textures[RendererTextures_Depth].get();
    m_secondaryTextures["HDR Texture"] = m_textures[RendererTextures_HDR].get();
    m_secondaryTextures["Normal Texture"] = m_textures[RendererTextures_Normal].get();
    m_secondaryTextures["Diffuse Texture"] = m_textures[RendererTextures_Diffuse].get();
    m_secondaryTextures["Specular Texture"] = m_textures[RendererTextures_Specular].get();
    m_secondaryTextures["OIT Accum"] = m_textures[RendererTextures_OITAccum].get();
    m_secondaryTextures["OIT Revealage"] = m_textures[RendererTextures_OITRevealage].get();
}

void ForwardRenderer::updateStepInternal( const ViewingParameters& renderData ) {
#ifndef NO_TRANSPARENCY
    m_transparentRenderObjects.clear();
    for ( auto it = m_fancyRenderObjects.begin(); it != m_fancyRenderObjects.end(); )
    {
        if ( (*it)->isTransparent() )
        {
            m_transparentRenderObjects.push_back( *it );
            it = m_fancyRenderObjects.erase( it );
        } else
        { ++it; }
    }
    m_fancyTransparentCount = m_transparentRenderObjects.size();
   // Question for Radiumv V2 Do we want ui too  ?
#endif
}

void ForwardRenderer::renderInternal( const ViewingParameters& renderData ) {

    m_fbo->bind();

    GL_ASSERT( glEnable( GL_DEPTH_TEST ) );
    GL_ASSERT( glDepthMask( GL_TRUE ) );
    GL_ASSERT( glColorMask( 1, 1, 1, 1 ) );

    GL_ASSERT( glDrawBuffers( 4, buffers ) );

    const auto clearColor = Core::Colors::FromChars<Core::Colorf>( 10, 10, 10, 0 );
    const auto clearZeros = Core::Colors::Black<Core::Colorf>();
    const auto clearOnes = Core::Colors::FromChars<Core::Colorf>( 255, 255, 255, 255 );
    const float clearDepth {1.0f};

    GL_ASSERT( glClearBufferfv( GL_COLOR, 0, clearColor.data() ) ); // Clear color
    GL_ASSERT( glClearBufferfv( GL_COLOR, 1, clearZeros.data() ) ); // Clear normals
    GL_ASSERT( glClearBufferfv( GL_COLOR, 2, clearZeros.data() ) ); // Clear diffuse
    GL_ASSERT( glClearBufferfv( GL_COLOR, 3, clearZeros.data() ) ); // Clear specular
    GL_ASSERT( glClearBufferfv( GL_DEPTH, 0, &clearDepth ) );       // Clear depth

    // Z prepass
    GL_ASSERT( glDepthFunc( GL_LESS ) );
    GL_ASSERT( glDisable( GL_BLEND ) );
    GL_ASSERT( glPointSize( 3.f ) );

    // Set in RenderParam the configuration about ambiant lighting (instead of hard constant
    // direclty in shaders)
    RenderParameters zprepassParams;
    for ( const auto& ro : m_fancyRenderObjects )
    {
        ro->render( zprepassParams, renderData, RenderTechnique::Z_PREPASS );
    }
#ifndef NO_TRANSPARENCY
    // Transparent objects are rendered in the Z-prepass, but only their fully opaque fragments (if any)
    // might influence the z-buffer
    // Rendering transparent objects assuming that they discard all their non-opaque fragments
    for (const auto &ro : m_transparentRenderObjects)
    {
        ro->render(zprepassParams, renderData, RenderTechnique::Z_PREPASS);
    }
#endif

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
                ro->render( lightingpassParams, renderData, RenderTechnique::LIGHTING_OPAQUE );
            }
#ifndef NO_TRANSPARENCY
            // Rendering transparent objects assuming that they discard all their non-opaque fragments
            for (const auto& ro : m_transparentRenderObjects)
            {
                ro->render( lightingpassParams, renderData, RenderTechnique::LIGHTING_OPAQUE);
            }
#endif
        }
    } else
    {
        LOG(logINFO) << "Opaque : no light sources, unable to render";
    }

#ifndef NO_TRANSPARENCY
    // Transparency (blending) pass
    if (!m_transparentRenderObjects.empty())
    {
        m_fbo->unbind();

        m_oitFbo->bind();

        GL_ASSERT(glDrawBuffers(2, buffers));
        GL_ASSERT(glClearBufferfv(GL_COLOR, 0, clearZeros.data()));
        GL_ASSERT(glClearBufferfv(GL_COLOR, 1, clearOnes.data()));

        GL_ASSERT(glDepthFunc(GL_LESS));
        GL_ASSERT(glEnable(GL_BLEND));

        GL_ASSERT(glBlendEquation(GL_FUNC_ADD));
        GL_ASSERT(glBlendFunci(0, GL_ONE, GL_ONE));
        GL_ASSERT(glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA));

        if (m_lightmanagers[0]->count() > 0)
        {
            // for ( const auto& l : m_lights )
            for ( size_t i = 0; i < m_lightmanagers[0]->count(); ++i)
            {
                const auto l = m_lightmanagers[0]->getLight(i);
                RenderParameters trasparencypassParams;
                l->getRenderParameters(trasparencypassParams);

                for (const auto &ro : m_transparentRenderObjects)
                {
                    ro->render(trasparencypassParams, renderData, RenderTechnique::LIGHTING_TRANSPARENT);
                }
            }
        }
        else
        {
            LOG(logINFO) << "Transparent : no light sources, unable to render";
        }

        m_oitFbo->unbind();

        m_fbo->bind();
        GL_ASSERT(glDrawBuffers(1, buffers));
        GL_ASSERT(glDisable( GL_DEPTH_TEST ) );
        GL_ASSERT(glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA));
        {
            auto shader = m_shaderMgr->getShaderProgram("ComposeOIT");
            shader->bind();
            shader->setUniform("u_OITSumColor", m_textures[RendererTextures_OITAccum].get(), 0);
            shader->setUniform("u_OITSumWeight", m_textures[RendererTextures_OITRevealage].get(), 1);
        }
        m_quadMesh->render();
        GL_ASSERT(glEnable( GL_DEPTH_TEST ) );
    }
#endif
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
                    ro->render( wireframepassParams, renderData, RenderTechnique::LIGHTING_OPAQUE );
                }
                // This will not work for the moment . skipping wireframe rendering of transparent objects
#if 0
                for ( const auto& ro : m_transparentRenderObjects)
                {
                    ro->render( wireframepassParams, viewingParameters, RenderTechnique::LIGHTING_OPAQUE );
                }
#endif
            }
        } else
        {
            LOG(logINFO) << "Wireframe : no light sources, unable to render";
        }

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
        const ShaderProgram* shader;

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
        for ( const auto& ro : m_xrayRenderObjects )
        {
            if ( ro->isVisible() )
            {
                shader = ro->getRenderTechnique()->getShader();

                // bind data
                shader->bind();
                // lighting for Xray : fixed
                shader->setUniform( "light.color", Ra::Core::Color(5.0, 5.0, 5.0, 1.0) );
                shader->setUniform( "light.type", Light::LightType::DIRECTIONAL );
                shader->setUniform( "light.directional.direction", Core::Vector3( 0, -1, 0 ) );

                Core::Matrix4 M = ro->getTransformAsMatrix();
                shader->setUniform( "transform.proj", renderData.projMatrix );
                shader->setUniform( "transform.view", renderData.viewMatrix );
                shader->setUniform( "transform.model", M );

                ro->getRenderTechnique()->getMaterial()->bind( shader );

                // render
                ro->getMesh()->render();
            }
        }
        m_uiXrayFbo->unbind();
    }
}

// Draw UI stuff, always drawn on top of everything else + clear ZMask
void ForwardRenderer::uiInternal( const ViewingParameters& renderData ) {
    const ShaderProgram* shader;

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
            shader = ro->getRenderTechnique()->getShader();

            // bind data
            shader->bind();

            Core::Matrix4 M = ro->getTransformAsMatrix();
            Core::Matrix4 MV = renderData.viewMatrix * M;
            Core::Vector3 V = MV.block<3, 1>( 0, 3 );
            Scalar d = V.norm();

            Core::Matrix4 S = Core::Matrix4::Identity();
            S.coeffRef( 0, 0 ) = S.coeffRef( 1, 1 ) = S.coeffRef( 2, 2 ) = d;

            M = M * S;

            shader->setUniform( "transform.proj", renderData.projMatrix );
            shader->setUniform( "transform.view", renderData.viewMatrix );
            shader->setUniform( "transform.model", M );

            ro->getRenderTechnique()->getMaterial()->bind( shader );

            // render
            ro->getMesh()->render();
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
    shader->setUniform( "gamma", 2.2 );
    m_quadMesh->render();

    GL_ASSERT( glDepthMask( GL_TRUE ) );
    GL_ASSERT( glEnable( GL_DEPTH_TEST ) );

    m_postprocessFbo->unbind();
}

void ForwardRenderer::resizeInternal() {
    m_pingPongSize = std::pow( uint(2),  uint( std::log2( std::min( m_width, m_height ) ) ) );

    m_textures[RendererTextures_Depth]->resize( m_width, m_height );
    m_textures[RendererTextures_HDR]->resize( m_width, m_height );
    m_textures[RendererTextures_Normal]->resize( m_width, m_height );
    m_textures[RendererTextures_Diffuse]->resize( m_width, m_height );
    m_textures[RendererTextures_Specular]->resize( m_width, m_height );
    m_textures[RendererTextures_OITAccum]->resize( m_width, m_height );
    m_textures[RendererTextures_OITRevealage]->resize( m_width, m_height );

    m_fbo->bind();
    m_fbo->attachTexture( GL_DEPTH_ATTACHMENT,
                          m_textures[RendererTextures_Depth]->texture() );
    m_fbo->attachTexture( GL_COLOR_ATTACHMENT0, m_textures[RendererTextures_HDR]->texture() );
    m_fbo->attachTexture( GL_COLOR_ATTACHMENT1,
                          m_textures[RendererTextures_Normal]->texture() );
    m_fbo->attachTexture( GL_COLOR_ATTACHMENT2,
                          m_textures[RendererTextures_Diffuse]->texture() );
    m_fbo->attachTexture( GL_COLOR_ATTACHMENT3,
                          m_textures[RendererTextures_Specular]->texture() );
    if ( m_fbo->checkStatus() != GL_FRAMEBUFFER_COMPLETE )
    {
        LOG( logERROR ) << "FBO Error (ForwardRenderer::m_fbo): " << m_fbo->checkStatus();
    }

#ifndef NO_TRANSPARENCY
    m_oitFbo->bind();
    m_oitFbo->attachTexture( GL_DEPTH_ATTACHMENT,
                             m_textures[RendererTextures_Depth]->texture() );
    m_oitFbo->attachTexture( GL_COLOR_ATTACHMENT0,
                             m_textures[RendererTextures_OITAccum]->texture() );
    m_oitFbo->attachTexture( GL_COLOR_ATTACHMENT1,
                             m_textures[RendererTextures_OITRevealage]->texture() );
    if ( m_fbo->checkStatus() != GL_FRAMEBUFFER_COMPLETE )
    {
        LOG( logERROR ) << "FBO Error (ForwardRenderer::m_oitFbo) : " << m_fbo->checkStatus();
    }
#endif

    m_postprocessFbo->bind();
    m_postprocessFbo->attachTexture( GL_DEPTH_ATTACHMENT,
                                     m_textures[RendererTextures_Depth]->texture() );
    m_postprocessFbo->attachTexture( GL_COLOR_ATTACHMENT0, m_fancyTexture->texture() );
    if ( m_fbo->checkStatus() != GL_FRAMEBUFFER_COMPLETE )
    {
        LOG( logERROR ) << "FBO Error (ForwardRenderer::m_postprocessFbo) : " << m_fbo->checkStatus();
    }

    // FIXED : when m_postprocessFbo use the RendererTextures_Depth, the depth buffer is erased and is therefore
    // useless for future computation. Do not use this post-process FBO to render eveything else than the scene.
    // Create several FBO with ther own configuration (uncomment Renderer::m_depthTexture->texture() to see the difference.)
    m_uiXrayFbo->bind();
    m_uiXrayFbo->attachTexture( GL_DEPTH_ATTACHMENT,
                              Renderer::m_depthTexture->texture() );
    m_uiXrayFbo->attachTexture( GL_COLOR_ATTACHMENT0, m_fancyTexture->texture() );
    if ( m_fbo->checkStatus() != GL_FRAMEBUFFER_COMPLETE )
    {
        LOG( logERROR ) << "FBO Error (ForwardRenderer::m_uiXrayFbo) : " << m_fbo->checkStatus();
    }
    // finished with fbo, undbind to bind default
    globjects::Framebuffer::unbind();
}

void ForwardRenderer::updateShadowMaps()
{
    // Radium V2 : implement shadow mapping
}

} // namespace Engine
} // namespace Ra
