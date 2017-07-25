
#include <Engine/Renderer/Renderers/TempRenderer.hpp>

#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Core/Log/Log.hpp>
#include <Core/Math/ColorPresets.hpp>
#include <Core/Containers/Algorithm.hpp>
#include <Core/Containers/MakeShared.hpp>

#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/OpenGL/OpenGL.hpp>
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

#include <globjects/Framebuffer.h>
// Only for debug purpose, not needed here
#include <globjects/Program.h>

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

        TempRenderer::TempRenderer( uint width, uint height )
                : Renderer(width, height)
        {

        }

        TempRenderer::~TempRenderer()
        {
            ShaderProgramManager::destroyInstance();
        }

        void TempRenderer::initializeInternal()
        {
            initShaders();
            initBuffers();

            DebugRender::createInstance();
            DebugRender::getInstance()->initialize();
        }

        void TempRenderer::initShaders()
        {
            m_shaderMgr->addShaderProgram("New", "Shaders/Simple.vert.glsl", "Shaders/Simple.frag.glsl");
            m_shaderMgr->addShaderProgram("Quad", "Shaders/Quad.vert.glsl", "Shaders/Quad.frag.glsl");
        }

        void TempRenderer::initBuffers()
        {
            LOG ( logDEBUG ) << "Main Framebuffer.";
            m_fbo.reset( new globjects::Framebuffer() );
            GL_ASSERT( glViewport( 0, 0, m_width, m_height ) );

            m_textures[RendererTextures_Normal].reset(new Texture("Normal Texture"));
            m_textures[RendererTextures_Normal]->internalFormat = GL_RGBA32F;
            m_textures[RendererTextures_Normal]->dataType = GL_FLOAT;
            m_secondaryTextures["Normal"] = m_textures[RendererTextures_Normal].get();

            m_textures[RendererTextures_Position].reset(new Texture("Position Texture"));
            m_textures[RendererTextures_Position]->internalFormat = GL_RGBA32F;
            m_textures[RendererTextures_Position]->dataType = GL_FLOAT;
            m_secondaryTextures["Position"] = m_textures[RendererTextures_Position].get();

            m_textures[RendererTextures_Quad].reset(new Texture("Quad Texture"));
            m_textures[RendererTextures_Quad]->internalFormat = GL_RGBA32F;
            m_textures[RendererTextures_Quad]->dataType = GL_FLOAT;
            m_secondaryTextures["Quad"] = m_textures[RendererTextures_Quad].get();
        }

        void TempRenderer::updateStepInternal( const RenderData& renderData )
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
#endif
        }

        void TempRenderer::renderInternal( const RenderData& renderData )
        {
            const ShaderProgram* shader;
            const Core::Colorf clearColor = Core::Colors::FromChars<Core::Colorf>(42, 42, 42, 0);
            const Core::Colorf clearZeros = Core::Colors::Black<Core::Colorf>();

            m_fbo->bind();

            GL_ASSERT( glColorMask( 1, 1, 1, 1 ) );
            GL_ASSERT( glDrawBuffers( 4, buffers ) );

            GL_ASSERT( glClearBufferfv( GL_COLOR, 0, clearColor.data() ) );   // Clear color
            GL_ASSERT( glClearBufferfv( GL_COLOR, 1, clearZeros.data() ) );   // Clear normals
            GL_ASSERT( glClearBufferfv( GL_COLOR, 2, clearZeros.data() ) );
            GL_ASSERT( glClearBufferfv( GL_COLOR, 3, clearZeros.data() ) );

            GL_ASSERT( glDepthFunc( GL_LESS) );
            GL_ASSERT( glDepthMask( GL_FALSE ) );

            GL_ASSERT( glEnable( GL_BLEND ) );
            GL_ASSERT( glBlendFunc( GL_ONE, GL_ONE ) );

            shader = m_shaderMgr->getShaderProgram("New");
            for ( const auto& l : m_lights )
            {
                RenderParameters params;
                l->getRenderParameters( params );

                for ( const auto& ro : m_fancyRenderObjects )
                {
                    ro->render(params, renderData, shader);
                }
            }
      //      GL_ASSERT( glDrawBuffers(1, buffers) );
        //    GL_ASSERT( glDepthMask( GL_FALSE ) );
          //  GL_ASSERT( glDepthFunc( GL_LESS) );
         //   GL_ASSERT( glEnable( GL_BLEND ) );
//            GL_ASSERT( glBlendFunc( GL_ONE, GL_ONE ) );

            shader = m_shaderMgr->getShaderProgram("Quad");
            shader->bind();
            shader->setUniform("color", m_textures[RendererTextures_Position].get(), 0);
            shader->setUniform("normal", m_textures[RendererTextures_Normal].get(), 1);
            shader->setUniform("WindowSize", Core::Vector2(m_width,m_height));


            m_quadMesh->render();
            // Restore state
            GL_ASSERT( glDepthFunc( GL_LESS ) );
            GL_ASSERT( glDisable( GL_BLEND ) );
            GL_ASSERT( glDepthMask( GL_TRUE ) );

            m_fbo->unbind();

        }

        // Draw debug stuff, do not overwrite depth map but do depth testing
        void TempRenderer::debugInternal( const RenderData& renderData )
        {}

        // Draw UI stuff, always drawn on top of everything else + clear ZMask
        void TempRenderer::uiInternal( const RenderData& renderData )
        {}

        void TempRenderer::postProcessInternal( const RenderData& renderData )
        {}

        void TempRenderer::resizeInternal()
        {
            m_textures[RendererTextures_Normal]->Generate(m_width, m_height, GL_RGBA);
            m_textures[RendererTextures_Position]->Generate(m_width, m_height, GL_RGBA);
            m_textures[RendererTextures_Quad]->Generate(m_width, m_height, GL_RGBA);

            m_fbo->bind();
            glViewport( 0, 0, m_width, m_height );

            m_fbo->attachTexture(GL_COLOR_ATTACHMENT0, m_fancyTexture.get()->texture());
            m_fbo->attachTexture(GL_COLOR_ATTACHMENT1, m_textures[RendererTextures_Normal].get()->texture());
            m_fbo->attachTexture(GL_COLOR_ATTACHMENT2, m_textures[RendererTextures_Position].get()->texture());
            m_fbo->attachTexture(GL_COLOR_ATTACHMENT3, m_textures[RendererTextures_Quad].get()->texture());
            if ( m_fbo->checkStatus() != GL_FRAMEBUFFER_COMPLETE )
            {
                LOG( logERROR ) << "FBO Error : " << m_fbo->checkStatus();
            }
            GL_CHECK_ERROR;

            // finished with fbo, undbind to bind default
            globjects::Framebuffer::unbind();
        }

    }
} // namespace Ra
