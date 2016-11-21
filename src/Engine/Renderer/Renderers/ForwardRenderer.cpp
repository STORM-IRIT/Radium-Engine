#include <Engine/Renderer/Renderers/ForwardRenderer.hpp>

#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Core/Log/Log.hpp>
#include <Core/Math/ColorPresets.hpp>
#include <Core/Containers/Algorithm.hpp>
#include <Core/Containers/MakeShared.hpp>

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

//#define NO_TRANSPARENCY
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
            : Renderer(width, height)
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
            m_shaderMgr->addShaderProgram("DepthMap", "../Shaders/DepthMap.vert.glsl", "../Shaders/DepthMap.frag.glsl");
            m_shaderMgr->addShaderProgram("DepthAmbientPass", "../Shaders/BlinnPhong.vert.glsl", "../Shaders/DepthAmbientPass.frag.glsl");
            m_shaderMgr->addShaderProgram("FinalCompose", "../Shaders/Basic2D.vert.glsl", "../Shaders/FinalCompose.frag.glsl");
#ifndef NO_TRANSPARENCY
            m_shaderMgr->addShaderProgram("LitOIT", "../Shaders/BlinnPhong.vert.glsl", "../Shaders/LitOIT.frag.glsl");
            m_shaderMgr->addShaderProgram("UnlitOIT", "../Shaders/Plain.vert.glsl", "../Shaders/UnlitOIT.frag.glsl");
            m_shaderMgr->addShaderProgram("ComposeOIT", "../Shaders/Basic2D.vert.glsl", "../Shaders/ComposeOIT.frag.glsl");
#endif
        }

        void ForwardRenderer::initBuffers()
        {
            m_fbo.reset(new FBO(FBO::Component_Color | FBO::Component_Depth, m_width, m_height));
            m_oitFbo.reset(new FBO(FBO::Component_Color | FBO::Component_Depth, m_width, m_height));
            m_postprocessFbo.reset(new FBO(FBO::Component_Color | FBO::Component_Depth, m_width, m_height));

            // Render pass
            m_textures[RendererTextures_Depth].reset(new Texture("Depth"));
            m_textures[RendererTextures_HDR].reset(new Texture("HDR"));
            m_textures[RendererTextures_Normal].reset(new Texture("Normal"));
            m_textures[RendererTextures_Diffuse].reset(new Texture("Diffuse"));
            m_textures[RendererTextures_Specular].reset(new Texture("Specular"));
            m_textures[RendererTextures_OITAccum].reset(new Texture("OIT Accum"));
            m_textures[RendererTextures_OITRevealage].reset(new Texture("OIT Revealage"));

            m_textures[RendererTextures_Depth]->internalFormat = GL_DEPTH_COMPONENT24;
            m_textures[RendererTextures_Depth]->dataType = GL_UNSIGNED_INT;

            m_textures[RendererTextures_HDR]->internalFormat = GL_RGBA32F;
            m_textures[RendererTextures_HDR]->dataType = GL_FLOAT;

            m_textures[RendererTextures_Normal]->internalFormat = GL_RGBA32F;
            m_textures[RendererTextures_Normal]->dataType = GL_FLOAT;

            m_textures[RendererTextures_Diffuse]->internalFormat = GL_RGBA32F;
            m_textures[RendererTextures_Diffuse]->dataType = GL_FLOAT;

            m_textures[RendererTextures_Specular]->internalFormat = GL_RGBA32F;
            m_textures[RendererTextures_Specular]->dataType = GL_FLOAT;

            m_textures[RendererTextures_OITAccum]->internalFormat = GL_RGBA32F;
            m_textures[RendererTextures_OITAccum]->dataType = GL_FLOAT;

            m_textures[RendererTextures_OITRevealage]->internalFormat = GL_RGBA32F;
            m_textures[RendererTextures_OITRevealage]->dataType = GL_FLOAT;

            m_secondaryTextures["Depth Texture"]    = m_textures[RendererTextures_Depth].get();
            m_secondaryTextures["HDR Texture"]      = m_textures[RendererTextures_HDR].get();
            m_secondaryTextures["Normal Texture"]   = m_textures[RendererTextures_Normal].get();
            m_secondaryTextures["Diffuse Texture"]  = m_textures[RendererTextures_Diffuse].get();
            m_secondaryTextures["Specular Texture"] = m_textures[RendererTextures_Specular].get();
            m_secondaryTextures["OIT Accum"]        = m_textures[RendererTextures_OITAccum].get();
            m_secondaryTextures["OIT Revealage"]    = m_textures[RendererTextures_OITRevealage].get();
        }

        void ForwardRenderer::updateStepInternal( const RenderData& renderData )
        {
#ifndef NO_TRANSPARENCY
            m_transparentRenderObjects.clear();

            for (auto it = m_fancyRenderObjects.begin(); it != m_fancyRenderObjects.end();)
            {
                std::shared_ptr<RenderObject> ro = *it;
                if (ro->isTransparent())
                {
                    m_transparentRenderObjects.push_back(ro);
                    it = m_fancyRenderObjects.erase(it);
                }
                else
                {
                    ++it;
                }
            }

            m_fancyTransparentCount = m_transparentRenderObjects.size();

            //Ra::Core::remove_copy_if(m_debugRenderObjects, m_transparentRenderObjects,
            //                         [](auto ro) { return ro->isTransparent(); });

            // FIXME(charly) Do we want ui too  ?
#endif
        }

        void ForwardRenderer::renderInternal( const RenderData& renderData )
        {
            const ShaderProgram* shader;

            m_fbo->useAsTarget();

            GL_ASSERT( glDepthMask( GL_TRUE ) );
            GL_ASSERT( glColorMask( 1, 1, 1, 1 ) );

            GL_ASSERT( glDrawBuffers( 4, buffers ) );

            const Core::Colorf clearColor = Core::Colors::FromChars<Core::Colorf>(42, 42, 42, 0);
            const Core::Colorf clearZeros = Core::Colors::Black<Core::Colorf>();
            const Core::Colorf clearOnes  = Core::Colors::FromChars<Core::Colorf>(255, 255, 255, 255);
            const float clearDepth( 1.0 );

            GL_ASSERT( glClearBufferfv( GL_COLOR, 0, clearColor.data() ) );   // Clear color
            GL_ASSERT( glClearBufferfv( GL_COLOR, 1, clearZeros.data() ) );   // Clear normals
            GL_ASSERT( glClearBufferfv( GL_COLOR, 2, clearZeros.data() ) );   // Clear diffuse
            GL_ASSERT( glClearBufferfv( GL_COLOR, 3, clearZeros.data() ) );   // Clear specular
            GL_ASSERT( glClearBufferfv( GL_DEPTH, 0, &clearDepth ) );         // Clear depth

            // Z prepass
            GL_ASSERT( glEnable( GL_DEPTH_TEST ) );
            GL_ASSERT( glDepthFunc( GL_LESS ) );
            GL_ASSERT( glDepthMask( GL_TRUE ) );

            GL_ASSERT( glDisable( GL_BLEND ) );

            GL_ASSERT( glDrawBuffers( 4, buffers ) );

            shader = m_shaderMgr->getShaderProgram("DepthAmbientPass");
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

            GL_ASSERT(glDrawBuffers(1, buffers));   // Draw color texture

            if ( m_lights.size() > 0 )
            {
                for ( const auto& l : m_lights )
                {
                    RenderParameters params;
                    l->getRenderParameters( params );

                    for ( const auto& ro : m_fancyRenderObjects )
                    {
                        ro->render(params, renderData);
                    }
                }
            }
            else
            {
                DirectionalLight l;
                l.setDirection( Core::Vector3( 0.3f, -1.0f, 0.0f ) );

                RenderParameters params;
                l.getRenderParameters( params );

                for ( const auto& ro : m_fancyRenderObjects )
                {
                    ro->render(params, renderData);
                }
            }

#ifndef NO_TRANSPARENCY
            m_fbo->unbind();
            m_oitFbo->useAsTarget();

            GL_ASSERT(glDrawBuffers(2, buffers));
            GL_ASSERT(glClearBufferfv(GL_COLOR, 0, clearZeros.data()));
            GL_ASSERT(glClearBufferfv(GL_COLOR, 1, clearOnes.data()));

            GL_ASSERT(glDepthFunc(GL_LESS));
            GL_ASSERT(glEnable(GL_BLEND));

            GL_ASSERT(glBlendEquation(GL_FUNC_ADD));
            GL_ASSERT(glBlendFunci(0, GL_ONE, GL_ONE));
            GL_ASSERT(glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA));

            shader = m_shaderMgr->getShaderProgram("LitOIT");
            shader->bind();

            if ( m_lights.size() > 0 )
            {
                for ( const auto& l : m_lights )
                {
                    RenderParameters params;
                    l->getRenderParameters( params );

                    for (size_t i = 0; i < m_fancyTransparentCount; ++i)
                    {
                        auto ro = m_transparentRenderObjects[i];
                        ro->render(params, renderData, shader);
                    }
                }
            }
            else
            {
                DirectionalLight l;
                l.setDirection( Core::Vector3( 0.3f, -1.0f, 0.0f ) );

                RenderParameters params;
                l.getRenderParameters( params );

                for (size_t i = 0; i < m_fancyTransparentCount; ++i)
                {
                    auto& ro = m_transparentRenderObjects[i];
                    ro->render(params, renderData, shader);
                }
            }

            m_oitFbo->unbind();

            m_fbo->useAsTarget();
            GL_ASSERT(glDrawBuffers(1, buffers));

            GL_ASSERT(glDepthFunc(GL_ALWAYS));
            GL_ASSERT(glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA));

            shader = m_shaderMgr->getShaderProgram("ComposeOIT");
            shader->bind();
            shader->setUniform("u_OITSumColor", m_textures[RendererTextures_OITAccum].get(), 0);
            shader->setUniform("u_OITSumWeight", m_textures[RendererTextures_OITRevealage].get(), 1);

            m_quadMesh->render();
#endif

            if (m_wireframe)
            {
                m_fbo->useAsTarget();

                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glEnable(GL_LINE_SMOOTH);
                glLineWidth(1.f);
                glEnable(GL_POLYGON_OFFSET_LINE);
                glPolygonOffset(-1.0f, -1.1f);

                // Light pass
                GL_ASSERT( glDepthFunc( GL_LEQUAL ) );
                GL_ASSERT( glDepthMask( GL_FALSE ) );

                GL_ASSERT( glEnable( GL_BLEND ) );
                GL_ASSERT( glBlendFunc( GL_ONE, GL_ONE ) );

                GL_ASSERT( glDrawBuffers( 1, buffers) );   // Draw color texture

                if ( m_lights.size() > 0 )
                {
                    for ( const auto& l : m_lights )
                    {
                        RenderParameters params;
                        l->getRenderParameters( params );

                        for ( const auto& ro : m_fancyRenderObjects )
                        {
                            ro->render(params, renderData);
                        }

                        for (size_t i = 0; i < m_fancyTransparentCount; ++i)
                        {
                            auto& ro = m_transparentRenderObjects[i];
                            ro->render(params, renderData);
                        }
                    }
                }
                else
                {
                    DirectionalLight l;
                    l.setDirection( Core::Vector3( 0.3f, -1.0f, 0.0f ) );

                    RenderParameters params;
                    l.getRenderParameters( params );

                    for ( const auto& ro : m_fancyRenderObjects )
                    {
                        ro->render(params, renderData);
                    }

                    for (size_t i = 0; i < m_fancyTransparentCount; ++i)
                    {
                        auto& ro = m_transparentRenderObjects[i];
                        ro->render(params, renderData);
                    }
                }

                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                glDisable(GL_POLYGON_OFFSET_LINE);
            }

            // Restore state
            GL_ASSERT( glDepthFunc( GL_LESS ) );
            GL_ASSERT( glDisable( GL_BLEND ) );
            GL_ASSERT( glDepthMask( GL_TRUE ) );
            GL_ASSERT( glDepthFunc( GL_LESS ) );

            m_fbo->unbind();
        }

        // Draw debug stuff, do not overwrite depth map but do depth testing
        void ForwardRenderer::debugInternal( const RenderData& renderData )
        {
            if ( m_drawDebug )
            {
                const ShaderProgram* shader;

                GL_ASSERT( glDisable( GL_BLEND ) );
                GL_ASSERT( glDepthMask( GL_FALSE ) );
                GL_ASSERT( glEnable( GL_DEPTH_TEST ) );
                GL_ASSERT( glDepthFunc( GL_LESS ) );

                m_postprocessFbo->useAsTarget( m_width, m_height );
                glDrawBuffers(1, buffers);

                glViewport(0, 0, m_width, m_height);

                for ( const auto& ro : m_debugRenderObjects )
                {
                     ro->render(RenderParameters{}, renderData);
                }

                DebugRender::getInstance()->render(renderData.viewMatrix,
                                                   renderData.projMatrix);

//#ifndef NO_TRANSPARENCY
#if 0
                m_postprocessFbo->unbind();
                m_oitFbo->useAsTarget();

                Core::Colorf clearZeros(0.0, 0.0, 0.0, 0.0);
                Core::Colorf clearOnes (1.0, 1.0, 1.0, 1.0);

                GL_ASSERT(glDrawBuffers(2, buffers));
                GL_ASSERT(glClearBufferfv(GL_COLOR, 0, clearZeros.data()));
                GL_ASSERT(glClearBufferfv(GL_COLOR, 1, clearOnes.data()));

                GL_ASSERT(glDepthFunc(GL_LESS));
                GL_ASSERT(glEnable(GL_BLEND));

                GL_ASSERT(glBlendEquation(GL_FUNC_ADD));
                GL_ASSERT(glBlendFunci(0, GL_ONE, GL_ONE));
                GL_ASSERT(glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA));

                shader = m_shaderMgr->getShaderProgram("UnlitOIT");
                shader->bind();

                for (size_t i = m_fancyTransparentCount; i < m_transparentRenderObjects.size(); ++i)
                {
                    auto ro = m_transparentRenderObjects[i];
                    ro->render(RenderParameters{}, renderData, shader);
                }

                m_oitFbo->unbind();

                m_postprocessFbo->useAsTarget();
                GL_ASSERT(glDrawBuffers(1, buffers + 1));

                GL_ASSERT(glDepthFunc(GL_ALWAYS));
                GL_ASSERT(glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA));

                shader = m_shaderMgr->getShaderProgram("ComposeOIT");
                shader->bind();
                shader->setUniform("u_OITSumColor", m_textures[TEX_OIT_TEXTURE_ACCUM].get(), 0);
                shader->setUniform("u_OITSumWeight", m_textures[TEX_OIT_TEXTURE_REVEALAGE].get(), 1);

                m_quadMesh->render();

                GL_ASSERT(glDisable(GL_BLEND));
#endif

                // Draw X rayed objects always on top of normal objects
                GL_ASSERT( glDepthMask( GL_TRUE ) );
                GL_ASSERT( glDepthFunc( GL_LESS ) );
                GL_ASSERT( glClear( GL_DEPTH_BUFFER_BIT ) );

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

                m_postprocessFbo->unbind();
            }
        }

        // Draw UI stuff, always drawn on top of everything else + clear ZMask
        void ForwardRenderer::uiInternal( const RenderData& renderData )
        {
            const ShaderProgram* shader;

            m_postprocessFbo->useAsTarget( m_width, m_height );

            glViewport(0, 0, m_width, m_height);
            glDrawBuffers(1, buffers);

            // Enable z-test
            if (1)
            {
                GL_ASSERT(glDepthMask(GL_TRUE));
                GL_ASSERT(glEnable(GL_DEPTH_TEST));
                GL_ASSERT(glDepthFunc(GL_LESS));
                GL_ASSERT(glClear(GL_DEPTH_BUFFER_BIT));
            }
            else
            {
                GL_ASSERT(glDepthMask(GL_FALSE));
                GL_ASSERT(glDisable(GL_DEPTH_TEST));
                GL_ASSERT(glDepthFunc(GL_LESS));
                GL_ASSERT(glClear(GL_DEPTH_BUFFER_BIT));
            }

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

            m_postprocessFbo->unbind();
        }

        void ForwardRenderer::postProcessInternal( const RenderData& renderData )
        {
            CORE_UNUSED( renderData );

            GL_ASSERT( glDisable(GL_DEPTH_TEST) );

            m_postprocessFbo->useAsTarget( m_width, m_height );

            GL_ASSERT( glColorMask( 1, 1, 1, 1 ) );
            glDepthMask(GL_FALSE);

            // FIXME(Charly): Do we really need to clear buffers ?
            GL_ASSERT( glClearColor( 1.0, 1.0, 0.0, 0.0 ) );
            GL_ASSERT( glDrawBuffers(5, buffers) );
            m_postprocessFbo->clear( FBO::Component( FBO::Component_Color) );

            const ShaderProgram* shader = nullptr;

            GL_ASSERT(glDrawBuffers(1, buffers));
            GL_ASSERT(glViewport(0, 0, m_width, m_height));
            shader = m_shaderMgr->getShaderProgram("DrawScreen");
            shader->bind();
            shader->setUniform("screenTexture", m_textures[RendererTextures_HDR].get(), 0);
            m_quadMesh->render();
        }

        void ForwardRenderer::resizeInternal()
        {
            m_pingPongSize = std::pow(2.0, Scalar(uint(std::log2(std::min(m_width, m_height)))));

            m_textures[RendererTextures_Depth]->Generate(m_width, m_height, GL_DEPTH_COMPONENT);
            m_textures[RendererTextures_HDR]->Generate(m_width, m_height, GL_RGBA);
            m_textures[RendererTextures_Normal]->Generate(m_width, m_height, GL_RGBA);
            m_textures[RendererTextures_Diffuse]->Generate(m_width, m_height, GL_RGBA);
            m_textures[RendererTextures_Specular]->Generate(m_width, m_height, GL_RGBA);
            m_textures[RendererTextures_OITAccum]->Generate(m_width, m_height, GL_RGBA);
            m_textures[RendererTextures_OITRevealage]->Generate(m_width, m_height, GL_RGBA);

            m_fbo->bind();
            m_fbo->setSize( m_width, m_height );
            m_fbo->attachTexture(GL_DEPTH_ATTACHMENT , m_textures[RendererTextures_Depth].get());
            m_fbo->attachTexture(GL_COLOR_ATTACHMENT0, m_textures[RendererTextures_HDR].get());
            m_fbo->attachTexture(GL_COLOR_ATTACHMENT1, m_textures[RendererTextures_Normal].get());
            m_fbo->attachTexture(GL_COLOR_ATTACHMENT2, m_textures[RendererTextures_Diffuse].get());
            m_fbo->attachTexture(GL_COLOR_ATTACHMENT3, m_textures[RendererTextures_Specular].get());
            m_fbo->check();
            m_fbo->unbind( true );

#ifndef NO_TRANSPARENCY
            m_oitFbo->bind();
            m_oitFbo->setSize( m_width, m_height );
            m_oitFbo->attachTexture(GL_DEPTH_ATTACHMENT , m_textures[RendererTextures_Depth].get());
            m_oitFbo->attachTexture(GL_COLOR_ATTACHMENT0, m_textures[RendererTextures_OITAccum].get());
            m_oitFbo->attachTexture(GL_COLOR_ATTACHMENT1, m_textures[RendererTextures_OITRevealage].get());
            m_oitFbo->check();
            m_oitFbo->unbind( true );
#endif

            m_postprocessFbo->bind();
            m_postprocessFbo->setSize(m_width, m_height);
            m_postprocessFbo->attachTexture(GL_DEPTH_ATTACHMENT , m_textures[RendererTextures_Depth].get());
            m_postprocessFbo->attachTexture(GL_COLOR_ATTACHMENT0, m_fancyTexture.get());
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
