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
            , m_fbo( nullptr )
            , m_postprocessFbo( nullptr )
            , m_pingPongFbo(nullptr)
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
            m_shaderMgr->addShaderProgram("DepthAmbientPass", "../Shaders/DepthAmbientPass.vert.glsl", "../Shaders/DepthAmbientPass.frag.glsl");
            m_shaderMgr->addShaderProgram("Luminance", "../Shaders/Basic2D.vert.glsl", "../Shaders/Luminance.frag.glsl");
            m_shaderMgr->addShaderProgram("Tonemapping", "../Shaders/Basic2D.vert.glsl", "../Shaders/Tonemapping.frag.glsl");
            m_shaderMgr->addShaderProgram("MinMax", "../Shaders/Basic2D.vert.glsl", "../Shaders/MinMax.frag.glsl");
            m_shaderMgr->addShaderProgram("Highpass", "../Shaders/Basic2D.vert.glsl", "../Shaders/Highpass.frag.glsl");
            m_shaderMgr->addShaderProgram("Blur", "../Shaders/Basic2D.vert.glsl", "../Shaders/Blur.frag.glsl");
            m_shaderMgr->addShaderProgram("FinalCompose", "../Shaders/Basic2D.vert.glsl", "../Shaders/FinalCompose.frag.glsl");
        }

        void ForwardRenderer::initBuffers()
        {
            m_fbo.reset( new FBO( FBO::Components( FBO::COLOR | FBO::DEPTH ), m_width, m_height ) );
            m_postprocessFbo.reset( new FBO( FBO::Components( FBO::COLOR | FBO::DEPTH), m_width, m_height ) );
            m_pingPongFbo.reset(new FBO(FBO::Components(FBO::COLOR), 1, 1));
            m_bloomFbo.reset(new FBO(FBO::Components(FBO::COLOR), m_width / 8, m_height / 8));

            // Render pass
            m_textures[TEX_DEPTH].reset( new Texture( "Depth", GL_TEXTURE_2D ) );
            m_textures[TEX_NORMAL].reset( new Texture( "Normal", GL_TEXTURE_2D ) );
            m_textures[TEX_LIT].reset( new Texture( "HDR", GL_TEXTURE_2D ) );
            m_textures[TEX_LUMINANCE].reset(new Texture("Luminance", GL_TEXTURE_2D));
            m_textures[TEX_TONEMAPPED].reset(new Texture("Tonemapped", GL_TEXTURE_2D));
            m_textures[TEX_BLOOM_PING].reset(new Texture("Bloom Ping", GL_TEXTURE_2D));
            m_textures[TEX_BLOOM_PONG].reset(new Texture("Bloom Pong", GL_TEXTURE_2D));
            m_textures[TEX_TONEMAP_PING].reset(new Texture("Minmax Ping", GL_TEXTURE_2D));
            m_textures[TEX_TONEMAP_PONG].reset(new Texture("Minmax Pong", GL_TEXTURE_2D));

            m_secondaryTextures["Depth Texture"]        = m_textures[TEX_DEPTH].get();
            m_secondaryTextures["Normal Texture"]       = m_textures[TEX_NORMAL].get();
            m_secondaryTextures["HDR Texture"]          = m_textures[TEX_LIT].get();
            m_secondaryTextures["Luminance Texture"]    = m_textures[TEX_LUMINANCE].get();
            m_secondaryTextures["Tonemapped Texture"]   = m_textures[TEX_TONEMAPPED].get();
            m_secondaryTextures["Bloom Texture 1"]      = m_textures[TEX_BLOOM_PING].get();
            m_secondaryTextures["Bloom Texture 2"]      = m_textures[TEX_BLOOM_PONG].get();
            m_secondaryTextures["Tonemaping Texture 1"] = m_textures[TEX_TONEMAP_PING].get();
            m_secondaryTextures["Tonemaping Texture 2"] = m_textures[TEX_TONEMAP_PONG].get();
        }

        void ForwardRenderer::updateStepInternal( const RenderData& renderData )
        {
            // Do nothing right now
        }

        void ForwardRenderer::renderInternal( const RenderData& renderData )
        {
            // FIXME(Charly): Do a bit of cleanup in the forward renderer
            // (e.g. Remove the "depth ambient pass")
            const ShaderProgram* shader;

            m_fbo->useAsTarget();

            GL_ASSERT( glDepthMask( GL_TRUE ) );
            GL_ASSERT( glColorMask( 1, 1, 1, 1 ) );

            GL_ASSERT( glDrawBuffers( 2, buffers ) );

            const Core::Colorf clearColor = Core::Colors::FromChars<Core::Colorf>(42, 42, 42, 0);
            const Core::Colorf clearZeros = Core::Colors::Black<Core::Colorf>();
            const float clearDepth( 1.0 );

            GL_ASSERT( glClearBufferfv( GL_COLOR, 0, clearZeros.data() ) );   // Clear normals
            GL_ASSERT( glClearBufferfv( GL_COLOR, 1, clearColor.data() ) );   // Clear color
            GL_ASSERT( glClearBufferfv( GL_DEPTH, 0, &clearDepth ) );         // Clear depth

            // Z prepass
            GL_ASSERT( glEnable( GL_DEPTH_TEST ) );
            GL_ASSERT( glDepthFunc( GL_LESS ) );
            GL_ASSERT( glDepthMask( GL_TRUE ) );

            GL_ASSERT( glDisable( GL_BLEND ) );

            GL_ASSERT( glDrawBuffers( 1, buffers ) );

            if (m_wireframe)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glEnable(GL_LINE_SMOOTH);
                glLineWidth(1.f);
                glEnable(GL_POLYGON_OFFSET_LINE);
                glPolygonOffset(-1.0f, -1.1f);
            }

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

            GL_ASSERT( glDrawBuffers( 1, buffers + 1 ) );   // Draw color texture

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

            if (m_wireframe)
            {
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
            const ShaderProgram* shader;

            GL_ASSERT( glDisable( GL_BLEND ) );
            GL_ASSERT( glDepthMask( GL_FALSE ) );
            GL_ASSERT( glEnable( GL_DEPTH_TEST ) );
            GL_ASSERT( glDepthFunc( GL_LESS ) );

            m_postprocessFbo->useAsTarget( m_width, m_height );
            glDrawBuffers(1, buffers);

            glViewport(0, 0, m_width, m_height);

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

             m_postprocessFbo->unbind();
        }

        // Draw UI stuff, always drawn on top of everything else + clear ZMask
        void ForwardRenderer::uiInternal( const RenderData& renderData )
        {
            const ShaderProgram* shader;
            const float          clearDepth( 1.0 );

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
            m_postprocessFbo->clear( FBO::Components( FBO::COLOR) );

            const ShaderProgram* shader = nullptr;

            if (m_postProcessEnabled)
            {
                // Get per pixel luminance
                GL_ASSERT(glDrawBuffers(1, buffers + 1));
                shader = m_shaderMgr->getShaderProgram("Luminance");
                shader->bind();
                shader->setUniform("hdr", m_textures[TEX_LIT].get(), 0);
                m_quadMesh->render();

                m_pingPongFbo->useAsTarget();

                uint size = m_pingPongSize;
                glDrawBuffers(1, buffers);
                glViewport(0, 0, size, size);
                shader = m_shaderMgr->getShaderProgram("DrawScreen");
                shader->bind();
                shader->setUniform("screenTexture", m_textures[TEX_LUMINANCE].get(), 0);
                m_quadMesh->render();

                // Get min / max / avg lum values
                shader = m_shaderMgr->getShaderProgram("MinMax");
                shader->bind();
                uint ping = 0;
                while (size != 1)
                {
                    size /= 2;
                    // Ping pong between textures
                    GL_ASSERT(glDrawBuffers(1, buffers + (ping + 1)%2));
                    GL_ASSERT(glViewport(0, 0, size, size));

                    shader->setUniform("color", m_textures[TEX_TONEMAP_PING + ping].get(), 0);
                    m_quadMesh->render();

                    ++ping %= 2;
                }

                Core::Color lum = m_textures[TEX_TONEMAP_PING + (ping + 1) % 2]->getTexel(0, 0);

                Scalar lumMin  = lum.x();
                Scalar lumMax  = lum.y();
                Scalar lumMean = std::exp(lum.z() / (m_pingPongSize * m_pingPongSize));

                m_postprocessFbo->useAsTarget(m_width, m_height);
                GL_ASSERT(glViewport(0, 0, m_width, m_height));

                // Do tonemapping
                GL_ASSERT(glDrawBuffers(1, buffers + 2));
                shader = m_shaderMgr->getShaderProgram("Tonemapping");
                shader->bind();
                shader->setUniform("hdr", m_textures[TEX_LIT].get(), 0);
                shader->setUniform("lumMin", lumMin);
                shader->setUniform("lumMax", lumMax);
                shader->setUniform("lumMean", lumMean);
                m_quadMesh->render();

                m_bloomFbo->useAsTarget();
                GL_ASSERT(glViewport(0, 0, m_width / 8, m_height / 8));
                GL_ASSERT(glDrawBuffers(1, buffers));
                shader = m_shaderMgr->getShaderProgram("Highpass");
                shader->bind();
                shader->setUniform("hdr", m_textures[TEX_LIT].get(), 0);
                shader->setUniform("lumMin", lumMin);
                shader->setUniform("lumMax", lumMax);
                shader->setUniform("lumMean", lumMean);
                m_quadMesh->render();

                // X blur
                shader = m_shaderMgr->getShaderProgram("Blur");
                shader->bind();

                for (uint i = 0; i < 2; ++i)
                {
                    GL_ASSERT(glDrawBuffers(1, buffers + 1));
                    shader->setUniform("color", m_textures[TEX_BLOOM_PING].get(), 0);
                    shader->setUniform("offset", Core::Vector2(8.0 / m_width, 0.0));
                    m_quadMesh->render();

                    GL_ASSERT(glDrawBuffers(1, buffers));
                    shader->setUniform("color", m_textures[TEX_BLOOM_PONG].get(), 0);
                    shader->setUniform("offset", Core::Vector2(0.0, 8.0 / m_height));
                    m_quadMesh->render();
                }

                // Compose final image (tonemapped + bloom + ...)
                m_postprocessFbo->useAsTarget(m_width, m_height);
                GL_ASSERT(glViewport(0, 0, m_width, m_height));
                GL_ASSERT(glDrawBuffers(1, buffers));

                shader = m_shaderMgr->getShaderProgram("FinalCompose");
                shader->bind();
                shader->setUniform("texColor", m_textures[TEX_TONEMAPPED].get(), 0);
                shader->setUniform("texBloom", m_textures[TEX_BLOOM_PING].get(), 1);
                m_quadMesh->render();

                GL_ASSERT( glDepthFunc( GL_LESS ) );
                m_postprocessFbo->unbind();
            }
            else
            {
                GL_ASSERT( glDrawBuffers(1, buffers) );
                GL_ASSERT( glViewport(0, 0, m_width, m_height) );
                shader = m_shaderMgr->getShaderProgram("DrawScreen");
                shader->bind();
                shader->setUniform("screenTexture", m_textures[TEX_LIT].get(), 0);
                m_quadMesh->render();
            }
        }

        void ForwardRenderer::resizeInternal()
        {
            m_pingPongSize = std::pow(2.0, Scalar(uint(std::log2(std::min(m_width, m_height)))));

            m_textures[TEX_DEPTH]->initGL(GL_DEPTH_COMPONENT24, m_width, m_height, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);
            m_textures[TEX_NORMAL]->initGL(GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr);
            m_textures[TEX_LIT]->initGL(GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr);
            m_textures[TEX_LUMINANCE]->initGL(GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr);
            m_textures[TEX_TONEMAPPED]->initGL(GL_RGBA32F, m_width, m_height, GL_RGBA, GL_FLOAT, nullptr);
            m_textures[TEX_TONEMAP_PING]->initGL(GL_RGBA32F, m_pingPongSize, m_pingPongSize, GL_RGBA, GL_FLOAT, nullptr);
            m_textures[TEX_TONEMAP_PONG]->initGL(GL_RGBA32F, m_pingPongSize, m_pingPongSize, GL_RGBA, GL_FLOAT, nullptr);
            m_textures[TEX_BLOOM_PING]->initGL(GL_RGBA32F, m_width / 8, m_height / 8, GL_RGBA, GL_FLOAT, nullptr);
            m_textures[TEX_BLOOM_PONG]->initGL(GL_RGBA32F, m_width / 8, m_height / 8, GL_RGBA, GL_FLOAT, nullptr);

            m_fbo->bind();
            m_fbo->setSize( m_width, m_height );
            m_fbo->attachTexture( GL_DEPTH_ATTACHMENT , m_textures[TEX_DEPTH].get() );
            m_fbo->attachTexture( GL_COLOR_ATTACHMENT0, m_textures[TEX_NORMAL].get() );
            m_fbo->attachTexture( GL_COLOR_ATTACHMENT1, m_textures[TEX_LIT].get() );
            m_fbo->check();
            m_fbo->unbind( true );

            m_postprocessFbo->bind();
            m_postprocessFbo->setSize( m_width, m_height );
            m_postprocessFbo->attachTexture(GL_DEPTH_ATTACHMENT , m_textures[TEX_DEPTH].get());
            m_postprocessFbo->attachTexture(GL_COLOR_ATTACHMENT0, m_fancyTexture.get());
            m_postprocessFbo->attachTexture(GL_COLOR_ATTACHMENT1, m_textures[TEX_LUMINANCE].get());
            m_postprocessFbo->attachTexture(GL_COLOR_ATTACHMENT2, m_textures[TEX_TONEMAPPED].get());
            m_postprocessFbo->check();
            m_postprocessFbo->unbind( true );

            m_pingPongFbo->bind();
            m_pingPongFbo->setSize(m_pingPongSize, m_pingPongSize);
            m_pingPongFbo->attachTexture(GL_COLOR_ATTACHMENT0, m_textures[TEX_TONEMAP_PING].get());
            m_pingPongFbo->attachTexture(GL_COLOR_ATTACHMENT1, m_textures[TEX_TONEMAP_PONG].get());
            m_pingPongFbo->check();
            m_pingPongFbo->unbind(true);

            m_bloomFbo->bind();
            m_bloomFbo->setSize(m_width / 8, m_height / 8);
            m_bloomFbo->attachTexture(GL_COLOR_ATTACHMENT0, m_textures[TEX_BLOOM_PING].get());
            m_bloomFbo->attachTexture(GL_COLOR_ATTACHMENT1, m_textures[TEX_BLOOM_PONG].get());
            m_bloomFbo->check();
            m_bloomFbo->unbind(true);

            GL_CHECK_ERROR;

            // Reset framebuffer state
            GL_ASSERT( glBindFramebuffer( GL_FRAMEBUFFER, 0 ) );

            GL_ASSERT( glDrawBuffer( GL_BACK ) );
            GL_ASSERT( glReadBuffer( GL_BACK ) );
        }
    }
} // namespace Ra
