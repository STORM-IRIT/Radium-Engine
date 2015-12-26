#include <Engine/Renderer/Renderers/ForwardRenderer.hpp>

#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Core/Log/Log.hpp>
#include <Core/Math/ColorPresets.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/OpenGL/OpenGL.hpp>
#include <Engine/Renderer/OpenGL/FBO.hpp>
#include <Engine/Renderer/RenderQueue/RenderQueue.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/RenderParameters.hpp>
#include <Engine/Renderer/Light/Light.hpp>
#include <Engine/Renderer/Light/DirLight.hpp>
#include <Engine/Renderer/Light/DirLight.hpp>
#include <Engine/Renderer/Light/PointLight.hpp>
#include <Engine/Renderer/Light/SpotLight.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/Texture/TextureManager.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>

#define NO_TRANSPARENCY
namespace Ra
{
    namespace Engine
    {

        namespace
        {
            const GLenum buffers[] =
            {
                GL_COLOR_ATTACHMENT0,
                GL_COLOR_ATTACHMENT1,
                GL_COLOR_ATTACHMENT2,
                GL_COLOR_ATTACHMENT3,
                GL_COLOR_ATTACHMENT4,
                GL_COLOR_ATTACHMENT5,
                GL_COLOR_ATTACHMENT6,
                GL_COLOR_ATTACHMENT7
            };
        }

        ForwardRenderer::ForwardRenderer( uint width, uint height )
            : Renderer( width, height )
            , m_depthAmbientShader( nullptr )
            , m_postprocessShader( nullptr )
            , m_quadMesh( nullptr )
            , m_fbo( nullptr )
            , m_postprocessFbo( nullptr )
        {
        }

        ForwardRenderer::~ForwardRenderer()
        {
            ShaderProgramManager::destroyInstance();
        }

        void ForwardRenderer::initializeInternal()
        {
            std::string shaderPath( "../Shaders" );
            std::string defaultShader( "Default" );

            m_shaderManager = ShaderProgramManager::createInstance( shaderPath, defaultShader );
            m_textureManager = TextureManager::createInstance();

            initShaders();
            initBuffers();

            Core::Vector4Array mesh;
            mesh.push_back( { Scalar( -1 ), Scalar( -1 ), Scalar( 0 ), Scalar( 0 ) } );
            mesh.push_back( { Scalar( -1 ), Scalar( 1 ), Scalar( 0 ), Scalar( 0 ) } );
            mesh.push_back( { Scalar( 1 ), Scalar( 1 ), Scalar( 0 ), Scalar( 0 ) } );
            mesh.push_back( { Scalar( 1 ), Scalar( -1 ), Scalar( 0 ), Scalar( 0 ) } );

            std::vector<uint> indices(
            {
                0, 1, 2,
                0, 3, 2
            } );

            m_quadMesh.reset( new Mesh( "quad" ) );
            m_quadMesh->loadGeometry( mesh, indices );
            m_quadMesh->updateGL();
        }

        void ForwardRenderer::initShaders()
        {
            m_depthAmbientShader            = m_shaderManager->addShaderProgram( "DepthAmbientPass" );
            m_renderpassCompositingShader   = m_shaderManager->addShaderProgram( "RenderPassCompose" );
            m_oiTransparencyShader          = m_shaderManager->addShaderProgram( "OITransparency" );
            m_postprocessShader             = m_shaderManager->addShaderProgram( "PostProcess" );
        }

        void ForwardRenderer::initBuffers()
        {
            m_fbo.reset( new FBO( FBO::Components( FBO::COLOR | FBO::DEPTH ), m_width, m_height ) );
            m_oitFbo.reset( new FBO( FBO::Components( FBO::COLOR | FBO::DEPTH ), m_width, m_height ) );
            m_postprocessFbo.reset( new FBO( FBO::Components( FBO::COLOR ), m_width, m_height ) );

            // Render pass
            m_renderpassTextures[RENDERPASS_TEXTURE_DEPTH]      .reset( new Texture( "Depth", GL_TEXTURE_2D ) );
            m_renderpassTextures[RENDERPASS_TEXTURE_AMBIENT]    .reset( new Texture( "Ambient", GL_TEXTURE_2D ) );
            m_renderpassTextures[RENDERPASS_TEXTURE_POSITION]   .reset( new Texture( "Position", GL_TEXTURE_2D ) );
            m_renderpassTextures[RENDERPASS_TEXTURE_NORMAL]     .reset( new Texture( "Normal", GL_TEXTURE_2D ) );
            m_renderpassTextures[RENDERPASS_TEXTURE_LIGHTED]    .reset( new Texture( "Color", GL_TEXTURE_2D ) );
            m_renderpassTexture                                 .reset( new Texture( "RenderPass", GL_TEXTURE_2D ) );

            // OIT pass
            m_oitTextures[OITPASS_TEXTURE_ACCUM]                .reset( new Texture( "OITAccum", GL_TEXTURE_2D ) );
            m_oitTextures[OITPASS_TEXTURE_REVEALAGE]            .reset( new Texture( "OITPRevealage", GL_TEXTURE_2D ) );

            // Picking

            // Post process pass
            m_finalTexture                                      .reset( new Texture( "Final", GL_TEXTURE_2D ) );

            resize( m_width, m_height );

            m_displayedTexture = m_finalTexture.get();
        }

        void ForwardRenderer::renderInternal( const RenderData& renderData )
        {
            m_fbo->useAsTarget();

            GL_ASSERT( glDepthMask( GL_TRUE ) );
            GL_ASSERT( glColorMask( 1, 1, 1, 1 ) );

            GL_ASSERT( glDrawBuffers( 6, buffers ) );

            const Core::Color clearColor = Core::Colors::FromChars(42, 42, 42, 128);
            const Core::Color clearZeros( 0.0, 0.0, 0.0, 0.0 );
            const Core::Color clearOnes( 1.0, 1.0, 1.0, 1.0 );
            const Scalar clearDepth( 1.0 );

            GL_ASSERT( glClearBufferfv( GL_COLOR, 0, clearColor.data() ) );   // Clear ambient
            GL_ASSERT( glClearBufferfv( GL_COLOR, 1, clearZeros.data() ) );   // Clear position
            GL_ASSERT( glClearBufferfv( GL_COLOR, 2, clearZeros.data() ) );   // Clear normal
            GL_ASSERT( glClearBufferfv( GL_COLOR, 3, clearOnes.data() ) );    // Clear picking
            GL_ASSERT( glClearBufferfv( GL_COLOR, 4, clearZeros.data() ) );   // Clear color
            GL_ASSERT( glClearBufferfv( GL_COLOR, 5, clearZeros.data() ) );   // Clear renderpass
            GL_ASSERT( glClearBufferfv( GL_DEPTH, 0, &clearDepth ) );   // Clear depth

            RenderParameters camParams;
            camParams.addParameter( "transform.view", renderData.viewMatrix );
            camParams.addParameter( "transform.proj", renderData.projMatrix );

            // Z + Ambient Prepass
            GL_ASSERT( glEnable( GL_DEPTH_TEST ) );
            GL_ASSERT( glDepthFunc( GL_LESS ) );
            GL_ASSERT( glDepthMask( GL_TRUE ) );

            GL_ASSERT( glDisable( GL_BLEND ) );

            GL_ASSERT( glDrawBuffers( 4, buffers ) );   // Draw ambient, position, normal, picking

            m_depthAmbientShader->bind();
            m_opaqueRenderQueue.render( m_depthAmbientShader, camParams );
#ifdef NO_TRANSPARENCY
            m_transparentRenderQueue.render( m_depthAmbientShader, camParams );
#endif

            // Light pass
            GL_ASSERT( glDepthFunc( GL_LEQUAL ) );
            GL_ASSERT( glDepthMask( GL_FALSE ) );

            GL_ASSERT( glEnable( GL_BLEND ) );
            GL_ASSERT( glBlendFunc( GL_ONE, GL_ONE ) );

            GL_ASSERT( glDrawBuffers( 1, buffers + 4 ) );   // Draw color texture

            if ( m_lights.size() > 0 )
            {
                for ( const auto& l : m_lights )
                {
                    // TODO(Charly): Light render params
                    RenderParameters params;
                    l->getRenderParameters( params );
                    params.concatParameters( camParams );
                    m_opaqueRenderQueue.render( params );
#ifdef NO_TRANSPARENCY
                    m_transparentRenderQueue.render( params );
#endif
                }
            }
            else
            {
                LOG( logINFO ) << "No light, ahah";
                DirectionalLight l;
                l.setDirection( Core::Vector3( 0.3, -1.0, 0.0 ) );

                RenderParameters params;
                l.getRenderParameters( params );
                params.concatParameters( camParams );
                m_opaqueRenderQueue.render( params );
#ifdef NO_TRANSPARENCY
                m_transparentRenderQueue.render( params );
#endif
            }

            m_fbo->unbind();

#ifndef NO_TRANSPARENCY
            m_oitFbo->useAsTarget();

            GL_ASSERT( glDrawBuffers( 2, buffers ) );

            // RT0 stores a sum, RT1 stores a product.
            GL_ASSERT( glClearBufferfv( GL_COLOR, 0, clearZeros.data() ) );
            GL_ASSERT( glClearBufferfv( GL_COLOR, 1, clearOnes.data() ) );

            GL_ASSERT( glDepthFunc( GL_LESS ) );
            GL_ASSERT( glEnable( GL_BLEND ) );
            GL_ASSERT( glBlendEquation( GL_FUNC_ADD ) );
            GL_ASSERT( glBlendFunci( 0, GL_ONE, GL_ONE ) );
            GL_ASSERT( glBlendFunci( 1, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA ) );

            m_oiTransparencyShader->bind();

            m_transparentRenderQueue.render( m_oiTransparencyShader );

            GL_ASSERT( glDisable( GL_BLEND ) );

            m_oitFbo->unbind();
#endif

            m_fbo->bind();
            // Draw debug stuff, do not overwrite depth map but do depth testing
            GL_ASSERT( glDrawBuffers( 1, buffers + 4 ) );

            GL_ASSERT( glDisable( GL_BLEND ) );
            GL_ASSERT( glDepthMask( GL_FALSE ) );
            GL_ASSERT( glEnable( GL_DEPTH_TEST ) );
            GL_ASSERT( glDepthFunc( GL_LESS ) );
            m_debugRenderQueue.render( camParams );

            // Draw X rayed objects always on top of normal objects
            GL_ASSERT( glDepthMask( GL_TRUE ) );
            GL_ASSERT( glClear( GL_DEPTH_BUFFER_BIT ) );
            GL_ASSERT( glClearBufferfv( GL_DEPTH, 0, &clearDepth ) );

            m_xrayRenderQueue.render( camParams );

            // Draw UI stuff, always drawn on top of everything else
            GL_ASSERT( glDepthMask( GL_TRUE ) );
            GL_ASSERT( glClear( GL_DEPTH_BUFFER_BIT ) );
            GL_ASSERT( glClearBufferfv( GL_DEPTH, 0, &clearDepth ) );

            m_uiRenderQueue.render( camParams );

            // Draw renderpass texture
            m_fbo->bind();
            GL_ASSERT( glDrawBuffers( 1, buffers + 5 ) );
            GL_ASSERT( glDepthFunc( GL_ALWAYS ) );
            m_fbo->unbind();

            m_renderpassCompositingShader->bind();

            m_renderpassCompositingShader->setUniform( "ambient", m_renderpassTextures[RENDERPASS_TEXTURE_AMBIENT].get(), 0 );
            m_renderpassCompositingShader->setUniform( "color", m_renderpassTextures[RENDERPASS_TEXTURE_LIGHTED].get(), 1 );
            m_renderpassCompositingShader->setUniform( "renderpass", 0 );
            m_quadMesh->render();

#ifndef NO_TRANSPARENCY
            GL_ASSERT( glEnable( GL_BLEND ) );
            GL_ASSERT( glBlendFunc( GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA ) );

            m_renderpassCompositingShader->setUniform( "oitSumColor", m_oitTextures[OITPASS_TEXTURE_ACCUM].get(), 2 );
            m_renderpassCompositingShader->setUniform( "oitSumWeight", m_oitTextures[OITPASS_TEXTURE_REVEALAGE].get(), 3 );
            m_renderpassCompositingShader->setUniform( "renderpass", 1 );
            m_quadMesh->render();
#endif

            GL_ASSERT( glDepthFunc( GL_LESS ) );

            GL_ASSERT( glDisable( GL_BLEND ) );
            GL_ASSERT( glDepthMask( GL_TRUE ) );
            GL_ASSERT( glDepthFunc( GL_LESS ) );

            m_fbo->unbind();
        }

        void ForwardRenderer::postProcessInternal( const RenderData& renderData )
        {
            CORE_UNUSED( renderData );

            // This pass does nothing by default

            m_postprocessFbo->useAsTarget( m_width, m_height );

            GL_ASSERT( glDepthMask( GL_TRUE ) );
            GL_ASSERT( glColorMask( 1, 1, 1, 1 ) );

            GL_ASSERT( glClearColor( 1.0, 0.0, 0.0, 0.0 ) );
            GL_ASSERT( glClearDepth( 0.0 ) );
            m_postprocessFbo->clear( FBO::Components( FBO::COLOR | FBO::DEPTH ) );

            // FIXME(Charly): Do we really need to clear the depth buffer ?
            GL_ASSERT( glDrawBuffers( 1, buffers ) );

            GL_ASSERT( glDepthFunc( GL_ALWAYS ) );

            m_postprocessShader->bind();
            m_postprocessShader->setUniform( "renderpassColor", m_renderpassTexture.get(), 0 );

            m_quadMesh->render();

            GL_ASSERT( glDepthFunc( GL_LESS ) );

            m_postprocessFbo->unbind();
        }

        void ForwardRenderer::resizeInternal()
        {
            if ( m_renderpassTextures[RENDERPASS_TEXTURE_DEPTH]->getId() != 0 )
            {
                m_renderpassTextures[RENDERPASS_TEXTURE_DEPTH]->deleteGL();
            }
            if ( m_renderpassTextures[RENDERPASS_TEXTURE_AMBIENT]->getId() != 0 )
            {
                m_renderpassTextures[RENDERPASS_TEXTURE_AMBIENT]->deleteGL();
            }
            if ( m_renderpassTextures[RENDERPASS_TEXTURE_POSITION]->getId() != 0 )
            {
                m_renderpassTextures[RENDERPASS_TEXTURE_POSITION]->deleteGL();
            }
            if ( m_renderpassTextures[RENDERPASS_TEXTURE_NORMAL]->getId() != 0 )
            {
                m_renderpassTextures[RENDERPASS_TEXTURE_NORMAL]->deleteGL();
            }
            if ( m_renderpassTextures[RENDERPASS_TEXTURE_LIGHTED]->getId() != 0 )
            {
                m_renderpassTextures[RENDERPASS_TEXTURE_LIGHTED]->deleteGL();
            }
            if ( m_renderpassTexture->getId() != 0 )
            {
                m_renderpassTexture->deleteGL();
            }

            if ( m_oitTextures[OITPASS_TEXTURE_ACCUM]->getId() != 0 )
            {
                m_oitTextures[OITPASS_TEXTURE_ACCUM]->deleteGL();
            }
            if ( m_oitTextures[OITPASS_TEXTURE_REVEALAGE]->getId() != 0 )
            {
                m_oitTextures[OITPASS_TEXTURE_REVEALAGE]->deleteGL();
            }

            m_renderpassTextures[RENDERPASS_TEXTURE_DEPTH]->initGL( GL_DEPTH_COMPONENT24, m_width, m_height, GL_DEPTH_COMPONENT,
                                                                    GL_UNSIGNED_INT, nullptr );
            m_renderpassTextures[RENDERPASS_TEXTURE_DEPTH]->setFilter( GL_LINEAR, GL_LINEAR );
            m_renderpassTextures[RENDERPASS_TEXTURE_DEPTH]->setClamp( GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER );

            m_renderpassTextures[RENDERPASS_TEXTURE_AMBIENT]->initGL( GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr );
            m_renderpassTextures[RENDERPASS_TEXTURE_AMBIENT]->setFilter( GL_LINEAR, GL_LINEAR );
            m_renderpassTextures[RENDERPASS_TEXTURE_AMBIENT]->setClamp( GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER );

            m_renderpassTextures[RENDERPASS_TEXTURE_POSITION]->initGL( GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr );
            m_renderpassTextures[RENDERPASS_TEXTURE_POSITION]->setFilter( GL_LINEAR, GL_LINEAR );
            m_renderpassTextures[RENDERPASS_TEXTURE_POSITION]->setClamp( GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER );

            m_renderpassTextures[RENDERPASS_TEXTURE_NORMAL]->initGL( GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr );
            m_renderpassTextures[RENDERPASS_TEXTURE_NORMAL]->setFilter( GL_LINEAR, GL_LINEAR );
            m_renderpassTextures[RENDERPASS_TEXTURE_NORMAL]->setClamp( GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER );

            m_renderpassTextures[RENDERPASS_TEXTURE_LIGHTED]->initGL( GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr );
            m_renderpassTextures[RENDERPASS_TEXTURE_LIGHTED]->setFilter( GL_LINEAR, GL_LINEAR );
            m_renderpassTextures[RENDERPASS_TEXTURE_LIGHTED]->setClamp( GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER );

            m_renderpassTexture->initGL( GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr );
            m_renderpassTexture->setFilter( GL_LINEAR, GL_LINEAR );
            m_renderpassTexture->setClamp( GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER );

            m_oitTextures[OITPASS_TEXTURE_ACCUM]->initGL( GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr );
            m_oitTextures[OITPASS_TEXTURE_ACCUM]->setFilter( GL_LINEAR, GL_LINEAR );
            m_oitTextures[OITPASS_TEXTURE_ACCUM]->setClamp( GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER );

            m_oitTextures[OITPASS_TEXTURE_REVEALAGE]->initGL( GL_R32F, m_width, m_height, GL_RED, GL_FLOAT, nullptr );
            m_oitTextures[OITPASS_TEXTURE_REVEALAGE]->setFilter( GL_LINEAR, GL_LINEAR );
            m_oitTextures[OITPASS_TEXTURE_REVEALAGE]->setClamp( GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER );

            m_fbo->bind();
            m_fbo->setSize( m_width, m_height );
            m_fbo->attachTexture( GL_DEPTH_ATTACHMENT , m_renderpassTextures[RENDERPASS_TEXTURE_DEPTH]   .get() );
            m_fbo->attachTexture( GL_COLOR_ATTACHMENT0, m_renderpassTextures[RENDERPASS_TEXTURE_AMBIENT] .get() );
            m_fbo->attachTexture( GL_COLOR_ATTACHMENT1, m_renderpassTextures[RENDERPASS_TEXTURE_POSITION].get() );
            m_fbo->attachTexture( GL_COLOR_ATTACHMENT2, m_renderpassTextures[RENDERPASS_TEXTURE_NORMAL]  .get() );
            m_fbo->attachTexture( GL_COLOR_ATTACHMENT4, m_renderpassTextures[RENDERPASS_TEXTURE_LIGHTED] .get() );
            m_fbo->attachTexture( GL_COLOR_ATTACHMENT5, m_renderpassTexture.get() );
            m_fbo->check();
            m_fbo->unbind( true );

            m_oitFbo->bind();
            m_oitFbo->setSize( m_width, m_height );
            m_oitFbo->attachTexture( GL_DEPTH_ATTACHMENT , m_renderpassTextures[RENDERPASS_TEXTURE_DEPTH].get() );
            m_oitFbo->attachTexture( GL_COLOR_ATTACHMENT0, m_oitTextures[OITPASS_TEXTURE_ACCUM]          .get() );
            m_oitFbo->attachTexture( GL_COLOR_ATTACHMENT1, m_oitTextures[OITPASS_TEXTURE_REVEALAGE]      .get() );
            m_fbo->check();
            m_fbo->unbind( true );

            m_postprocessFbo->bind();
            m_postprocessFbo->setSize( m_width, m_height );
            m_postprocessFbo->attachTexture( GL_COLOR_ATTACHMENT0, m_finalTexture.get() );
            m_postprocessFbo->check();
            m_postprocessFbo->unbind( true );

            GL_CHECK_ERROR;

            // Reset framebuffer state
            GL_ASSERT( glBindFramebuffer( GL_FRAMEBUFFER, 0 ) );

            GL_ASSERT( glDrawBuffer( GL_BACK ) );
            GL_ASSERT( glReadBuffer( GL_BACK ) );
        }

        void ForwardRenderer::debugTexture( uint texIdx )
        {
            if ( texIdx > RENDERPASS_TEXTURE_COUNT )
            {
                m_displayedTexture = m_finalTexture.get();
                m_displayedIsDepth = false;
            }
            else
            {
                m_displayedTexture = m_renderpassTextures[texIdx].get();
                m_displayedIsDepth = ( texIdx == 0 );
            }
        }
    }
} // namespace Ra
