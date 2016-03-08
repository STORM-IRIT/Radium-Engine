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
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/RenderTechnique/Material.hpp>
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
#include <Engine/Renderer/Renderers/DebugRender.hpp>

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
            initShaders();
            initBuffers();

            DebugRender::createInstance();
            DebugRender::getInstance()->initialize();
        }

        void ForwardRenderer::initShaders()
        {
            m_depthAmbientShader = m_shaderManager->addShaderProgram( "DepthAmbientPass" );
            m_postprocessShader  = m_shaderManager->addShaderProgram( "PostProcess" );
        }

        void ForwardRenderer::initBuffers()
        {
            m_fbo.reset( new FBO( FBO::Components( FBO::COLOR | FBO::DEPTH ), m_width, m_height ) );
            m_postprocessFbo.reset( new FBO( FBO::Components( FBO::COLOR ), m_width, m_height ) );

            // Render pass
            m_textures[TEX_DEPTH]  .reset( new Texture( "Depth", GL_TEXTURE_2D ) );
            m_textures[TEX_NORMAL] .reset( new Texture( "Normal", GL_TEXTURE_2D ) );
            m_textures[TEX_LIT].reset( new Texture( "Color", GL_TEXTURE_2D ) );

            m_secondaryTextures["Depth Texture"]   = m_textures[TEX_DEPTH].get();
            m_secondaryTextures["Normal Texture"]  = m_textures[TEX_NORMAL].get();
            m_secondaryTextures["Lit Texture"] = m_textures[TEX_LIT].get();
        }

        void ForwardRenderer::updateStepInternal( const RenderData& renderData )
        {
            // Do nothing right now
        }

        void ForwardRenderer::renderInternal( const RenderData& renderData )
        {
            // FIXME(Charly): Do a bit of cleanup in the forward renderer
            // (e.g. Remove the "depth ambient pass")
            ShaderProgram* shader;

            m_fbo->useAsTarget();

            GL_ASSERT( glDepthMask( GL_TRUE ) );
            GL_ASSERT( glColorMask( 1, 1, 1, 1 ) );

            GL_ASSERT( glDrawBuffers( 2, buffers ) );

            const Core::Color clearColor = Core::Colors::FromChars(42, 42, 42, 0);
            const Core::Color clearZeros( 0.0, 0.0, 0.0, 0.0 );
            const Scalar clearDepth( 1.0 );

            GL_ASSERT( glClearBufferfv( GL_COLOR, 0, clearZeros.data() ) );   // Clear normals
            GL_ASSERT( glClearBufferfv( GL_COLOR, 1, clearColor.data() ) );   // Clear color
            GL_ASSERT( glClearBufferfv( GL_DEPTH, 0, &clearDepth ) );   // Clear depth

            // Z prepass
            GL_ASSERT( glEnable( GL_DEPTH_TEST ) );
            GL_ASSERT( glDepthFunc( GL_LESS ) );
            GL_ASSERT( glDepthMask( GL_TRUE ) );

            GL_ASSERT( glDisable( GL_BLEND ) );

            GL_ASSERT( glDrawBuffers( 1, buffers ) );

            shader = m_depthAmbientShader;
            shader->bind();
            for ( const auto& ro : m_fancyRenderObjects )
            {
                if ( ro->isVisible() )
                {
                    // bind data
                    Core::Matrix4 M = ro->getTransformAsMatrix();
                    Core::Matrix4 N = M.inverse().transpose();

                    shader->setUniform( "transform.proj", renderData.projMatrix );
                    shader->setUniform( "transform.view", renderData.viewMatrix );
                    shader->setUniform( "transform.model", M );
                    shader->setUniform( "transform.worldNormal", N );

                    ro->getRenderTechnique()->material->bind( shader );

                    // render
                    ro->getMesh()->render();
                }
            }


            // Light pass
            GL_ASSERT( glDepthFunc( GL_LEQUAL ) );
            GL_ASSERT( glDepthMask( GL_FALSE ) );

            GL_ASSERT( glEnable( GL_BLEND ) );
            GL_ASSERT( glBlendFunc( GL_ONE, GL_ONE ) );

            GL_ASSERT( glDrawBuffers( 1, buffers + 1 ) );   // Draw color texture

            if ( m_lights.size() > 0 )
            {
                for ( const auto& l : m_lights )
                {
                    // TODO(Charly): Light render params
                    RenderParameters params;
                    l->getRenderParameters( params );

                    for ( const auto& ro : m_fancyRenderObjects )
                    {
                        if ( ro->isVisible() )
                        {
                            shader = ro->getRenderTechnique()->shader;

                            // bind data
                            shader->bind();

                            Core::Matrix4 M = ro->getTransformAsMatrix();
                            Core::Matrix4 N = M.inverse().transpose();

                            shader->setUniform( "transform.proj", renderData.projMatrix );
                            shader->setUniform( "transform.view", renderData.viewMatrix );
                            shader->setUniform( "transform.model", M );
                            shader->setUniform( "transform.worldNormal", N );
                            params.bind( shader );

                            ro->getRenderTechnique()->material->bind( shader );

                            // render
                            ro->getMesh()->render();
                        }
                    }
                }
            }
            else
            {
                DirectionalLight l;
                l.setDirection( Core::Vector3( 0.3, -1.0, 0.0 ) );

                RenderParameters params;
                l.getRenderParameters( params );

                for ( const auto& ro : m_fancyRenderObjects )
                {
                    shader = ro->getRenderTechnique()->shader;

                    // bind data
                    shader->bind();
                    shader->setUniform( "proj", renderData.projMatrix );
                    shader->setUniform( "view", renderData.viewMatrix );
                    shader->setUniform( "model", ro->getLocalTransformAsMatrix() );
                    params.bind( shader );

                    ro->getRenderTechnique()->material->bind( shader );

                    // render
                    ro->getMesh()->render();
                }
            }

            // Draw debug stuff, do not overwrite depth map but do depth testing
            GL_ASSERT( glDisable( GL_BLEND ) );
            GL_ASSERT( glDepthMask( GL_FALSE ) );
            GL_ASSERT( glEnable( GL_DEPTH_TEST ) );
            GL_ASSERT( glDepthFunc( GL_LESS ) );
            if ( m_drawDebug )
            {
                for ( const auto& ro : m_debugRenderObjects )
                {
                    if ( ro->isVisible() )
                    {
                        shader = ro->getRenderTechnique()->shader;

                        // bind data
                        shader->bind();

                        Core::Matrix4 M = ro->getTransformAsMatrix();
                        shader->setUniform( "transform.proj", renderData.projMatrix );
                        shader->setUniform( "transform.view", renderData.viewMatrix );
                        shader->setUniform( "transform.model", M );

                        ro->getRenderTechnique()->material->bind( shader );

                        // render
                        ro->getMesh()->render();
                    }
                }

                DebugRender::getInstance()->render(renderData.viewMatrix, 
                                                   renderData.projMatrix);

            }

            // Draw X rayed objects always on top of normal objects
            GL_ASSERT( glDepthMask( GL_TRUE ) );
            GL_ASSERT( glClear( GL_DEPTH_BUFFER_BIT ) );
            GL_ASSERT( glClearBufferfv( GL_DEPTH, 0, &clearDepth ) );
            if ( m_drawDebug )
            {
                for ( const auto& ro : m_xrayRenderObjects )
                {
                    if ( ro->isVisible() )
                    {
                        shader = ro->getRenderTechnique()->shader;

                        // bind data
                        shader->bind();

                        Core::Matrix4 M = ro->getTransformAsMatrix();
                        shader->setUniform( "transform.proj", renderData.projMatrix );
                        shader->setUniform( "transform.view", renderData.viewMatrix );
                        shader->setUniform( "transform.model", M );

                        ro->getRenderTechnique()->material->bind( shader );

                        // render
                        ro->getMesh()->render();
                    }
                }
            }

            // Draw UI stuff, always drawn on top of everything else
            GL_ASSERT( glDepthMask( GL_TRUE ) );
            GL_ASSERT( glClear( GL_DEPTH_BUFFER_BIT ) );
            GL_ASSERT( glClearBufferfv( GL_DEPTH, 0, &clearDepth ) );
            for ( const auto& ro : m_uiRenderObjects )
            {
                if ( ro->isVisible() )
                {
                    shader = ro->getRenderTechnique()->shader;

                    // bind data
                    shader->bind();

                    Core::Matrix4 M = ro->getTransformAsMatrix();
                    Core::Matrix4 MV = renderData.viewMatrix * M;
                    Core::Vector3 V = MV.block<3, 1>( 0, 3 );
                    Scalar d = V.norm();

                    Core::Matrix4 S = Core::Matrix4::Identity();
                    S( 0, 0 ) = S( 1, 1 ) = S( 2, 2 ) = d;

                    M = M * S;

                    shader->setUniform( "transform.proj", renderData.projMatrix );
                    shader->setUniform( "transform.view", renderData.viewMatrix );
                    shader->setUniform( "transform.model", M );

                    ro->getRenderTechnique()->material->bind( shader );

                    // render
                    ro->getMesh()->render();
                }
            }

            // Restore state
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
            m_postprocessShader->setUniform( "renderpassColor", m_textures[TEX_LIT].get(), 0 );

            m_quadMesh->render();

            GL_ASSERT( glDepthFunc( GL_LESS ) );

            m_postprocessFbo->unbind();
        }

        void ForwardRenderer::resizeInternal()
        {
            if ( m_textures[TEX_DEPTH]->getId() != 0 )
            {
                m_textures[TEX_DEPTH]->deleteGL();
            }
            if ( m_textures[TEX_NORMAL]->getId() != 0 )
            {
                m_textures[TEX_NORMAL]->deleteGL();
            }
            if ( m_textures[TEX_LIT]->getId() != 0 )
            {
                m_textures[TEX_LIT]->deleteGL();
            }

            m_textures[TEX_DEPTH]->initGL( GL_DEPTH_COMPONENT24, m_width, m_height, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr );
            m_textures[TEX_DEPTH]->setFilter( GL_LINEAR, GL_LINEAR );
            m_textures[TEX_DEPTH]->setClamp( GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER );

            m_textures[TEX_NORMAL]->initGL( GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr );
            m_textures[TEX_NORMAL]->setFilter( GL_LINEAR, GL_LINEAR );
            m_textures[TEX_NORMAL]->setClamp( GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER );

            m_textures[TEX_LIT]->initGL( GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr );
            m_textures[TEX_LIT]->setFilter( GL_LINEAR, GL_LINEAR );
            m_textures[TEX_LIT]->setClamp( GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER );

            m_fbo->bind();
            m_fbo->setSize( m_width, m_height );
            m_fbo->attachTexture( GL_DEPTH_ATTACHMENT , m_textures[TEX_DEPTH]   .get() );
            m_fbo->attachTexture( GL_COLOR_ATTACHMENT0, m_textures[TEX_NORMAL]  .get() );
            m_fbo->attachTexture( GL_COLOR_ATTACHMENT1, m_textures[TEX_LIT] .get() );
            m_fbo->check();
            m_fbo->unbind( true );

            m_postprocessFbo->bind();
            m_postprocessFbo->setSize( m_width, m_height );
            m_postprocessFbo->attachTexture( GL_COLOR_ATTACHMENT0, m_fancyTexture.get() );
            m_postprocessFbo->check();
            m_postprocessFbo->unbind( true );

            GL_CHECK_ERROR;

            // Reset framebuffer state
            GL_ASSERT( glBindFramebuffer( GL_FRAMEBUFFER, 0 ) );

            GL_ASSERT( glDrawBuffer( GL_BACK ) );
            GL_ASSERT( glReadBuffer( GL_BACK ) );
        }
    }
} // namespace Ra
