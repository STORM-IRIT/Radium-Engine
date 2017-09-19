#include <Engine/Renderer/Renderers/TempRenderer.hpp>

#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Core/Log/Log.hpp>
#include <Core/Math/ColorPresets.hpp>
#include <Core/Containers/Algorithm.hpp>
#include <Core/Containers/MakeShared.hpp>
#include <Core/Mesh/MeshTypes.hpp>

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
            LOG(logDEBUG) << "1";
            m_strokeMesh.reset(new Mesh("stroke"));
            LOG(logDEBUG) << "2";
            quadVertices = new Core::Vector3f[4];
            quadVertices[0] = Core::Vector3f(0,0,0);
            quadVertices[1] = Core::Vector3f(1,0,0);
            quadVertices[2] = Core::Vector3f(1,1,0);
            quadVertices[3] = Core::Vector3f(0,1,0);
            LOG(logDEBUG) << "3";
            quadTex = new Core::Vector4f[4];
            quadTex[0] = Core::Vector4f(0,0,0,0);
            quadTex[1] = Core::Vector4f(1,0,0,0);
            quadTex[2] = Core::Vector4f(1,1,0,0);
            quadTex[3] = Core::Vector4f(0,1,0,0);
            LOG(logDEBUG) << "4";
            DebugRender::createInstance();
            DebugRender::getInstance()->initialize();
            LOG(logDEBUG) << "5";
        }

        void TempRenderer::initShaders()
        {
            ShaderConfiguration configPointCloud("FirstPass");
            configPointCloud.addShader(ShaderType_VERTEX,   "Shaders/Simple.vert.glsl");
            configPointCloud.addShader(ShaderType_GEOMETRY, "Shaders/Splatt.geom.glsl");
            configPointCloud.addShader(ShaderType_FRAGMENT, "Shaders/Simple.frag.glsl");
            ShaderConfigurationFactory::addConfiguration(configPointCloud);
            m_shaderMgr->addShaderProgram(configPointCloud);
            m_shaderMgr->addShaderProgram("MaskPass", "Shaders/Basic2D.vert.glsl", "Shaders/mask.frag.glsl");
            m_shaderMgr->addShaderProgram("SecondPass", "Shaders/Basic2D.vert.glsl", "Shaders/Quad.frag.glsl");
            m_shaderMgr->addShaderProgram("imp2", "Shaders/Basic2D.vert.glsl", "Shaders/implicit02.frag.glsl");
            m_shaderMgr->addShaderProgram("imp3", "Shaders/Basic2D.vert.glsl", "Shaders/implicit03.frag.glsl");
            m_shaderMgr->addShaderProgram("sing", "Shaders/Basic2D.vert.glsl", "Shaders/singularity.frag.glsl");
            m_shaderMgr->addShaderProgram("imp5", "Shaders/Basic2D.vert.glsl", "Shaders/implicit05.frag.glsl");
            m_shaderMgr->addShaderProgram("filter", "Shaders/Basic2D.vert.glsl", "Shaders/implicit06.frag.glsl");
            m_shaderMgr->addShaderProgram("disp", "Shaders/Basic2D.vert.glsl", "Shaders/implicit07.frag.glsl");
            m_shaderMgr->addShaderProgram("stroke", "Shaders/implicitStroke.vert.glsl", "Shaders/implicitStroke.frag.glsl");
        }

        void TempRenderer::initBuffers()
        {
            LOG ( logDEBUG ) << "Main Framebuffer.";
            m_fbo.reset( new globjects::Framebuffer() );
            GL_ASSERT( glViewport( 0, 0, m_width, m_height ) );

            LOG ( logDEBUG ) << "Mask Framebuffer.";
            m_maskFbo.reset( new globjects::Framebuffer() );
            GL_ASSERT( glViewport( 0, 0, m_width, m_height ) );

            LOG ( logDEBUG ) << "Fit Framebuffer.";
            m_fitFbo.reset( new globjects::Framebuffer() );
            GL_ASSERT( glViewport( 0, 0, m_width, m_height ) );

            LOG ( logDEBUG ) << "even smooth Framebuffer.";
            m_evenSmoothFbo.reset( new globjects::Framebuffer() );
            GL_ASSERT( glViewport( 0, 0, m_width, m_height ) );

            LOG ( logDEBUG ) << "odd smooth Framebuffer.";
            m_oddSmoothFbo.reset( new globjects::Framebuffer() );
            GL_ASSERT( glViewport( 0, 0, m_width, m_height ) );

            LOG ( logDEBUG ) << "imp3 Framebuffer.";
            m_imp3Fbo.reset( new globjects::Framebuffer() );
            GL_ASSERT( glViewport( 0, 0, m_width, m_height ) );

            LOG ( logDEBUG ) << "sing Framebuffer.";
            m_singFbo.reset( new globjects::Framebuffer() );
            GL_ASSERT( glViewport( 0, 0, m_width, m_height ) );

            LOG ( logDEBUG ) << "analysis Framebuffer.";
            m_analysisFbo.reset( new globjects::Framebuffer() );
            GL_ASSERT( glViewport( 0, 0, m_width, m_height ) );

            LOG ( logDEBUG ) << "filter Framebuffer.";
            m_filterFbo.reset( new globjects::Framebuffer() );
            GL_ASSERT( glViewport( 0, 0, m_width, m_height ) );

            LOG ( logDEBUG ) << "disp Framebuffer.";
            m_dispFbo.reset( new globjects::Framebuffer() );
            GL_ASSERT( glViewport( 0, 0, m_width, m_height ) );

            LOG ( logDEBUG ) << "stroke Framebuffer.";
            m_strokeFbo.reset( new globjects::Framebuffer() );
            GL_ASSERT( glViewport( 0, 0, m_width, m_height ) );

            m_textures[RendererTextures_Depth].reset(new Texture("Depth"));
            m_textures[RendererTextures_Depth]->internalFormat = GL_DEPTH_COMPONENT24;
            m_textures[RendererTextures_Depth]->dataType = GL_UNSIGNED_INT;
            //m_secondaryTextures["Depth Texture"] = m_textures[RendererTextures_Depth].get();

            m_textures[RendererTextures_Normal].reset(new Texture("Normal Texture"));
            m_textures[RendererTextures_Normal]->internalFormat = GL_RGBA32F;
            m_textures[RendererTextures_Normal]->dataType = GL_FLOAT;
            m_secondaryTextures["1-Normal"] = m_textures[RendererTextures_Normal].get();

            m_textures[RendererTextures_Position].reset(new Texture("Position Texture"));
            m_textures[RendererTextures_Position]->internalFormat = GL_RGBA32F;
            m_textures[RendererTextures_Position]->dataType = GL_FLOAT;
            m_secondaryTextures["1-Position"] = m_textures[RendererTextures_Position].get();

            m_textures[RendererTextures_ESmooth].reset(new Texture("Even Smooth Texture"));
            m_textures[RendererTextures_ESmooth]->internalFormat = GL_RGBA32F;
            m_textures[RendererTextures_ESmooth]->dataType = GL_FLOAT;
            m_secondaryTextures["2-Smooth Even"] = m_textures[RendererTextures_ESmooth].get();

            m_textures[RendererTextures_OSmooth].reset(new Texture("Odd Smooth Texture"));
            m_textures[RendererTextures_OSmooth]->internalFormat = GL_RGBA32F;
            m_textures[RendererTextures_OSmooth]->dataType = GL_FLOAT;
            m_secondaryTextures["2-Smooth Odd"] = m_textures[RendererTextures_OSmooth].get();

            m_textures[RendererTextures_FitPos].reset(new Texture("Fit Pos Texture"));
            m_textures[RendererTextures_FitPos]->internalFormat = GL_RGBA32F;
            m_textures[RendererTextures_FitPos]->dataType = GL_FLOAT;
            m_secondaryTextures["2-Fit Position"] = m_textures[RendererTextures_FitPos].get();

            m_textures[RendererTextures_Shade].reset(new Texture("Shading Texture"));
            m_textures[RendererTextures_Shade]->internalFormat = GL_RGBA32F;
            m_textures[RendererTextures_Shade]->dataType = GL_FLOAT;
            m_secondaryTextures["2-Shade"] = m_textures[RendererTextures_Shade].get();

            m_textures[RendererTextures_Mask].reset(new Texture("Mask Texture"));
            m_textures[RendererTextures_Mask]->internalFormat = GL_RGBA32F;
            m_textures[RendererTextures_Mask]->dataType = GL_FLOAT;
            m_secondaryTextures["1-Mask"] = m_textures[RendererTextures_Mask].get();

            m_textures[RendererTextures_ThirdColor].reset(new Texture("Third Color Texture"));
            m_textures[RendererTextures_ThirdColor]->internalFormat = GL_RGBA32F;
            m_textures[RendererTextures_ThirdColor]->dataType = GL_FLOAT;
            m_secondaryTextures["3-Color"] = m_textures[RendererTextures_ThirdColor].get();

            m_textures[RendererTextures_ThirdData].reset(new Texture("Third Data Texture"));
            m_textures[RendererTextures_ThirdData]->internalFormat = GL_RGBA32F;
            m_textures[RendererTextures_ThirdData]->dataType = GL_FLOAT;
            m_secondaryTextures["3-Data"] = m_textures[RendererTextures_ThirdData].get();

            m_textures[RendererTextures_Sing].reset(new Texture("Forth With Sing Texture"));
            m_textures[RendererTextures_Sing]->internalFormat = GL_RGBA32F;
            m_textures[RendererTextures_Sing]->dataType = GL_FLOAT;
            m_secondaryTextures["4-Singularities"] = m_textures[RendererTextures_Sing].get();

            m_textures[RendererTextures_Analysis1].reset(new Texture("Analysis 1 Texture"));
            m_textures[RendererTextures_Analysis1]->internalFormat = GL_RGBA32F;
            m_textures[RendererTextures_Analysis1]->dataType = GL_FLOAT;
            m_secondaryTextures["5-Analysis1"] = m_textures[RendererTextures_Analysis1].get();

            m_textures[RendererTextures_Analysis2].reset(new Texture("Analysis 2 Texture"));
            m_textures[RendererTextures_Analysis2]->internalFormat = GL_RGBA32F;
            m_textures[RendererTextures_Analysis2]->dataType = GL_FLOAT;
            m_secondaryTextures["5-Analysis2"] = m_textures[RendererTextures_Analysis2].get();

            m_textures[RendererTextures_Analysis3].reset(new Texture("Analysis 3 Texture"));
            m_textures[RendererTextures_Analysis3]->internalFormat = GL_RGBA32F;
            m_textures[RendererTextures_Analysis3]->dataType = GL_FLOAT;
            m_secondaryTextures["5-Analysis3"] = m_textures[RendererTextures_Analysis3].get();

            m_textures[RendererTextures_Filter].reset(new Texture("Filter Texture"));
            m_textures[RendererTextures_Filter]->internalFormat = GL_RGBA32F;
            m_textures[RendererTextures_Filter]->dataType = GL_FLOAT;
            m_secondaryTextures["6-Filter"] = m_textures[RendererTextures_Filter].get();

            m_textures[RendererTextures_Disp].reset(new Texture("disp Texture"));
            m_textures[RendererTextures_Disp]->internalFormat = GL_RGBA32F;
            m_textures[RendererTextures_Disp]->dataType = GL_FLOAT;
            m_secondaryTextures["7-Disp"] = m_textures[RendererTextures_Disp].get();

            m_textures[RendererTextures_Stroke].reset(new Texture("disp Texture"));
            m_textures[RendererTextures_Stroke]->internalFormat = GL_RGBA32F;
            m_textures[RendererTextures_Stroke]->dataType = GL_FLOAT;
            m_secondaryTextures["8-Stroke"] = m_textures[RendererTextures_Stroke].get();

            m_textures[RendererTextures_Eye].reset(new Texture("eye Texture"));
            m_textures[RendererTextures_Eye]->internalFormat = GL_RGBA32F;
            m_textures[RendererTextures_Eye]->dataType = GL_FLOAT;
            //m_secondaryTextures["Eye"] = m_textures[RendererTextures_Eye].get();

            m_textures[RendererTextures_Ks].reset(new Texture("ks Texture"));
            m_textures[RendererTextures_Ks]->internalFormat = GL_RGBA32F;
            m_textures[RendererTextures_Ks]->dataType = GL_FLOAT;
           // m_secondaryTextures["Ks"] = m_textures[RendererTextures_Ks].get();

            m_textures[RendererTextures_Kd].reset(new Texture("kd Texture"));
            m_textures[RendererTextures_Kd]->internalFormat = GL_RGBA32F;
            m_textures[RendererTextures_Kd]->dataType = GL_FLOAT;
            //m_secondaryTextures["Kd"] = m_textures[RendererTextures_Kd].get();

            m_textures[RendererTextures_Ns].reset(new Texture("ns Texture"));
            m_textures[RendererTextures_Ns]->internalFormat = GL_RGBA32F;
            m_textures[RendererTextures_Ns]->dataType = GL_FLOAT;
            //m_secondaryTextures["Ns"] = m_textures[RendererTextures_Ns].get();

            m_textures[RendererTextures_Noise].reset(TextureManager::getInstance()->addTexture("Shaders/noise.jpg"));
            m_textures[RendererTextures_Noise]->internalFormat = GL_RGBA32F;
            m_textures[RendererTextures_Noise]->dataType = GL_FLOAT;

            m_textures[RendererTextures_Brush].reset(TextureManager::getInstance()->addTexture("Shaders/brush.jpg"));
            m_textures[RendererTextures_Brush]->internalFormat = GL_RGBA32F;
            m_textures[RendererTextures_Brush]->dataType = GL_FLOAT;
           // m_secondaryTextures["Noise"] = m_textures[RendererTextures_Noise].get();
        }

        void TempRenderer::updateStepInternal( const RenderData& renderData )
        {}

        void TempRenderer::renderInternal( const RenderData& renderData )
        {
            const ShaderProgram* shader;
            const Core::Colorf clearColor = Core::Colors::FromChars<Core::Colorf>(42, 42, 42, 0);
            const Core::Colorf clearWhite = Core::Colors::White<Core::Colorf>();
            const Core::Colorf clearZeros = Core::Colors::Black<Core::Colorf>();
            const float clearDepth( 1.0 );

            m_fbo->bind();

            GL_ASSERT( glColorMask( 1, 1, 1, 1 ) );
            GL_ASSERT( glDrawBuffers( 6, buffers ) );

            GL_ASSERT( glClearBufferfv( GL_COLOR, 0, clearColor.data() ) );   // Clear color
            GL_ASSERT( glClearBufferfv( GL_COLOR, 1, clearZeros.data() ) );   // Clear normals
            GL_ASSERT( glClearBufferfv( GL_COLOR, 2, clearColor.data() ) );
            GL_ASSERT( glClearBufferfv( GL_COLOR, 3, clearColor.data() ) );
            GL_ASSERT( glClearBufferfv( GL_COLOR, 4, clearZeros.data() ) );
            GL_ASSERT( glClearBufferfv( GL_COLOR, 5, clearColor.data() ) );
            GL_ASSERT( glClearBufferfv( GL_DEPTH, 0, &clearDepth ) );         // Clear depth

//1sh pass
            GL_ASSERT( glDepthMask( GL_TRUE ) );
            GL_ASSERT( glEnable( GL_DEPTH_TEST ) );
            GL_ASSERT( glDepthFunc( GL_LESS) );
            GL_ASSERT( glDisable( GL_BLEND ) );

            shader = m_shaderMgr->getShaderProgram("FirstPass");
            shader->setUniform("radius", m_radius);

            for ( const auto& ro : m_fancyRenderObjects )
            {
                if(ro->isVisible())
                {
                    Core::Matrix4 M = ro->getTransformAsMatrix();
                    Core::Matrix4 N = M.inverse().transpose();

                    // bind data
                    shader->bind();
                    shader->setUniform("transform.proj", renderData.projMatrix);
                    shader->setUniform("transform.view", renderData.viewMatrix);
                    shader->setUniform("transform.model", M);
                    shader->setUniform("transform.worldNormal", N);
                    ro->getRenderTechnique()->material->bind(shader);
                    // render
                    ro->getMesh()->render();
                }
            }
            m_fbo->unbind();

// mask pass
            m_maskFbo->bind();

            GL_ASSERT( glColorMask( 1, 1, 1, 1 ) );
            GL_ASSERT( glDrawBuffers( 1, buffers ) );
            GL_ASSERT( glClearBufferfv( GL_COLOR, 0, clearColor.data() ) );   // Clear color

            GL_ASSERT( glDisable( GL_DEPTH_TEST ) );
            GL_ASSERT( glDisable( GL_BLEND ) );

            shader = m_shaderMgr->getShaderProgram("MaskPass");
            shader->bind();
            shader->setUniform("neighSize", m_neighSize);
            shader->setUniform("position", m_textures[RendererTextures_Position].get(), 0);
            m_quadMesh->render();
            m_maskFbo->unbind();
// 2nd pass
            m_fitFbo->bind();

            GL_ASSERT( glColorMask( 1, 1, 1, 1 ) );
            GL_ASSERT( glDrawBuffers( 3, buffers ) );
            GL_ASSERT( glClearBufferfv( GL_COLOR, 0, clearColor.data() ) );   // Clear color
            GL_ASSERT( glClearBufferfv( GL_COLOR, 1, clearColor.data() ) );
            GL_ASSERT( glClearBufferfv( GL_COLOR, 2, clearColor.data() ) );
           // GL_ASSERT( glClearBufferfv( GL_COLOR, 3, clearColor.data() ) );

            GL_ASSERT( glDisable( GL_DEPTH_TEST ) );
            GL_ASSERT( glDisable( GL_BLEND ) );

            shader = m_shaderMgr->getShaderProgram("SecondPass");
            shader->bind();
            shader->setUniform("position", m_textures[RendererTextures_Position].get(), 0);
            shader->setUniform("normal", m_textures[RendererTextures_Normal].get(), 1);
            shader->setUniform("mask", m_textures[RendererTextures_Mask].get(), 2);
            shader->setUniform("eye", m_textures[RendererTextures_Eye].get(), 3);
            shader->setUniform("ks", m_textures[RendererTextures_Ks].get(), 4);
            shader->setUniform("kd", m_textures[RendererTextures_Kd].get(), 5);
            shader->setUniform("ns", m_textures[RendererTextures_Ns].get(), 6);
            shader->setUniform("showPos", m_showPos);
            shader->setUniform("planeFit", m_planeFit);
            shader->setUniform("depthThresh", m_dThresh);
            shader->setUniform("neighSize", m_neighSize);
            shader->setUniform("depthCalc", m_depthCalc);

            RenderParameters params;
            for ( const auto& l : m_lights )
            {
                l->getRenderParameters(params);
                params.bind(shader);
                //getRenderTechnique()->material->bind(shader);
                m_quadMesh->render();
            }
            m_fitFbo->unbind();

// implicit02 Pass

            for (int i=0; i<m_smoothNum; i++)
            {
                i%2 == 0 ? m_evenSmoothFbo->bind() : m_oddSmoothFbo->bind();

                GL_ASSERT(glColorMask(1, 1, 1, 1));
                GL_ASSERT(glDrawBuffers(1, buffers));
                GL_ASSERT(glClearBufferfv(GL_COLOR, 0, clearColor.data()));   // Clear color

                GL_ASSERT(glDisable(GL_DEPTH_TEST));
                GL_ASSERT(glDisable(GL_BLEND));

                shader = m_shaderMgr->getShaderProgram("imp2");
                shader->bind();
                shader->setUniform("grad", m_textures[(i%2 == 0 ? RendererTextures_OSmooth : RendererTextures_ESmooth)].get(), 0);

                m_quadMesh->render();
                i%2 == 0 ? m_evenSmoothFbo->unbind() : m_oddSmoothFbo->unbind();
            }

// implicit03 Pass
            m_imp3Fbo->bind();

            GL_ASSERT( glColorMask( 1, 1, 1, 1 ) );
            GL_ASSERT( glDrawBuffers( 2, buffers ) );
            GL_ASSERT( glClearBufferfv( GL_COLOR, 0, clearColor.data() ) );   // Clear color
            GL_ASSERT( glClearBufferfv( GL_COLOR, 1, clearColor.data() ) );

            GL_ASSERT( glDisable( GL_DEPTH_TEST ) );
            GL_ASSERT( glDisable( GL_BLEND ) );

            shader = m_shaderMgr->getShaderProgram("imp3");
            shader->bind();
            shader->setUniform("grad", m_textures[(m_smoothNum%2 == 0 ? RendererTextures_OSmooth : RendererTextures_ESmooth)].get(), 0);

            m_quadMesh->render();
            m_imp3Fbo->unbind();

// implicit04 Pass
   /*         m_imp4Fbo->bind();

            GL_ASSERT( glColorMask( 1, 1, 1, 1 ) );
            GL_ASSERT( glDrawBuffers( 1, buffers ) );
            GL_ASSERT( glClearBufferfv( GL_COLOR, 0, clearColor.data() ) );   // Clear color

            GL_ASSERT( glDisable( GL_DEPTH_TEST ) );
            GL_ASSERT( glDisable( GL_BLEND ) );

            shader = m_shaderMgr->getShaderProgram("imp4");
            shader->bind();
            shader->setUniform("desc1", m_textures[RendererTextures_ThirdColor].get(), 0);

            m_quadMesh->render();
            m_imp4Fbo->unbind();
*/
//Singularity check

            m_singFbo->bind();

            GL_ASSERT( glColorMask( 1, 1, 1, 1 ) );
            GL_ASSERT( glDrawBuffers( 1, buffers ) );
            GL_ASSERT( glClearBufferfv( GL_COLOR, 0, clearColor.data() ) );   // Clear color

            GL_ASSERT( glDisable( GL_DEPTH_TEST ) );
            GL_ASSERT( glDisable( GL_BLEND ) );

            shader = m_shaderMgr->getShaderProgram("sing");
            shader->bind();
            shader->setUniform("grad", m_textures[(m_smoothNum%2 == 0 ? RendererTextures_OSmooth : RendererTextures_ESmooth)].get(), 0);
            shader->setUniform("desc2", m_textures[RendererTextures_ThirdData].get(), 1);

            m_quadMesh->render();
            m_singFbo->unbind();

//implicit 05

            m_analysisFbo->bind();

            GL_ASSERT( glColorMask( 1, 1, 1, 1 ) );
            GL_ASSERT( glDrawBuffers( 3, buffers ) );
            GL_ASSERT( glClearBufferfv( GL_COLOR, 0, clearColor.data() ) );   // Clear color
            GL_ASSERT( glClearBufferfv( GL_COLOR, 1, clearColor.data() ) );   // Clear color
            GL_ASSERT( glClearBufferfv( GL_COLOR, 2, clearColor.data() ) );   // Clear color

            GL_ASSERT( glDisable( GL_DEPTH_TEST ) );
            GL_ASSERT( glDisable( GL_BLEND ) );



            constexpr int  n=9;
            Eigen::Matrix<Scalar, n, 4> X, H;
            //MatNMf X  = distanceMatrix(n);
            //MatNMf Xt = X.transpose();

            const int half = n/2;

            for(int i=0;i<n;++i) {
                const float x = (float)(i-half);

                // CUBIC distances
                X(i,0) = x*x*x;
                X(i,1) = x*x;
                X(i,2) = x;
                X(i,3) = 1.0f;
            }

            H = ((X.transpose()*X).inverse() * X.transpose()).transpose();

            shader = m_shaderMgr->getShaderProgram("imp5");
            shader->bind();
            shader->setUniform("desc1", m_textures[RendererTextures_ThirdColor].get(), 0);
            shader->setUniform("desc2", m_textures[RendererTextures_ThirdData].get(), 1);
            shader->setUniform("sing", m_textures[RendererTextures_Sing].get(), 2);
            shader->setUniform("H0", Core::Vector4f(H.row(0)));
            shader->setUniform("H1", Core::Vector4f(H.row(1)));
            shader->setUniform("H2", Core::Vector4f(H.row(2)));
            shader->setUniform("H3", Core::Vector4f(H.row(3)));
            shader->setUniform("H4", Core::Vector4f(H.row(4)));
            shader->setUniform("H5", Core::Vector4f(H.row(5)));
            shader->setUniform("H6", Core::Vector4f(H.row(6)));
            shader->setUniform("H7", Core::Vector4f(H.row(7)));
            shader->setUniform("H8", Core::Vector4f(H.row(8)));

            m_quadMesh->render();
            m_analysisFbo->unbind();

//filtering

            m_filterFbo->bind();

            GL_ASSERT( glColorMask( 1, 1, 1, 1 ) );
            GL_ASSERT( glDrawBuffers( 1, buffers ) );
            GL_ASSERT( glClearBufferfv( GL_COLOR, 0, clearColor.data() ) );   // Clear color

            GL_ASSERT( glDisable( GL_DEPTH_TEST ) );
            GL_ASSERT( glDisable( GL_BLEND ) );

            shader = m_shaderMgr->getShaderProgram("filter");
            shader->bind();
            shader->setUniform("analysis1", m_textures[RendererTextures_Analysis1].get(), 0);
            shader->setUniform("analysis2", m_textures[RendererTextures_Analysis2].get(), 1);
            shader->setUniform("analysis3", m_textures[RendererTextures_Analysis3].get(), 2);
            shader->setUniform("desc1", m_textures[RendererTextures_ThirdColor].get(), 3);
            shader->setUniform("desc2", m_textures[RendererTextures_ThirdData].get(), 4);
            shader->setUniform("grad", m_textures[(m_smoothNum%2 == 0 ? RendererTextures_OSmooth : RendererTextures_ESmooth)].get(), 5);
            shader->setUniform("shading", m_textures[RendererTextures_Shade].get(), 6);
            shader->setUniform("sing", m_textures[RendererTextures_Sing].get(), 7);
            shader->setUniform("H0", Core::Vector4f(H.row(0)));
            shader->setUniform("H1", Core::Vector4f(H.row(1)));
            shader->setUniform("H2", Core::Vector4f(H.row(2)));
            shader->setUniform("H3", Core::Vector4f(H.row(3)));
            shader->setUniform("H4", Core::Vector4f(H.row(4)));
            shader->setUniform("H5", Core::Vector4f(H.row(5)));
            shader->setUniform("H6", Core::Vector4f(H.row(6)));
            shader->setUniform("H7", Core::Vector4f(H.row(7)));
            shader->setUniform("H8", Core::Vector4f(H.row(8)));
            shader->setUniform("sw", 1.0/m_width);
            shader->setUniform("sh", 1.0/m_height);


            m_quadMesh->render();
            m_filterFbo->unbind();
//disp
            m_dispFbo->bind();

            GL_ASSERT( glColorMask( 1, 1, 1, 1 ) );
            GL_ASSERT( glDrawBuffers( 1, buffers ) );
            GL_ASSERT( glClearBufferfv( GL_COLOR, 0, clearColor.data() ) );   // Clear color

            GL_ASSERT( glDisable( GL_DEPTH_TEST ) );
            GL_ASSERT( glDisable( GL_BLEND ) );

            m_textures[RendererTextures_Noise]->bind(9);

            shader = m_shaderMgr->getShaderProgram("disp");
            shader->bind();
            shader->setUniform("grad", m_textures[(m_smoothNum%2 == 0 ? RendererTextures_OSmooth : RendererTextures_ESmooth)].get(), 0);
            shader->setUniform("shading", m_textures[RendererTextures_Shade].get(), 1);
            shader->setUniform("desc1", m_textures[RendererTextures_ThirdColor].get(), 2);
            shader->setUniform("desc2", m_textures[RendererTextures_ThirdData].get(), 3);
            shader->setUniform("analysis1", m_textures[RendererTextures_Analysis1].get(), 4);
            shader->setUniform("analysis2", m_textures[RendererTextures_Analysis2].get(), 5);
            shader->setUniform("analysis3", m_textures[RendererTextures_Analysis3].get(), 6);
            shader->setUniform("analysis4", m_textures[RendererTextures_Filter].get(), 7);
            shader->setUniform("analysis5", m_textures[RendererTextures_FitPos].get(), 8);
            shader->setUniform("noise", m_textures[RendererTextures_Noise].get(), 9);
            shader->setUniform("sing", m_textures[RendererTextures_Sing].get(), 10);

            m_quadMesh->render();
            m_dispFbo->unbind();
// stroke
            m_strokeFbo->bind();

            GL_ASSERT( glColorMask( 1, 1, 1, 1 ) );
            GL_ASSERT( glDrawBuffers( 1, buffers ) );
            GL_ASSERT( glClearBufferfv( GL_COLOR, 0, clearWhite.data() ) );   // Clear color

            GL_ASSERT( glDisable( GL_DEPTH_TEST ) );
            GL_ASSERT( glDisable( GL_BLEND ) );

            shader = m_shaderMgr->getShaderProgram("stroke");
            shader->bind();
            shader->setUniform("wh", Core::Vector2f(m_width, m_height));
            shader->setUniform("texData1", m_textures[RendererTextures_Analysis1].get(), 0);
            shader->setUniform("texData2", m_textures[RendererTextures_Analysis2].get(), 1);
            shader->setUniform("texData3", m_textures[RendererTextures_Analysis3].get(), 2);
            shader->setUniform("texData4", m_textures[RendererTextures_Filter].get(), 3);
            shader->setUniform("texData5", m_textures[RendererTextures_FitPos].get(), 4);
            shader->setUniform("texBrush", m_textures[RendererTextures_Brush].get(), 6);

            shader->setUniform("transform.proj", renderData.projMatrix);
            m_strokeMesh->render();
            m_strokeFbo->unbind();

// Restore state
            GL_ASSERT( glEnable( GL_DEPTH_TEST ) );
            GL_ASSERT( glDepthFunc( GL_LESS ) );
            GL_ASSERT( glDisable( GL_BLEND ) );
            GL_ASSERT( glDepthMask( GL_TRUE ) );

            m_fbo->unbind();

        }
        void TempRenderer::setShowPos(bool showPos)
        {
            m_showPos = showPos;
        }
        void TempRenderer::setNeighSize(int neighSize)
        {
            m_neighSize = neighSize;
        }
        void TempRenderer::setDepthThresh(double dThresh)
        {
            m_dThresh = dThresh;
        }
        void TempRenderer::setPlaneFit(bool planeFit)
        {
            m_planeFit = planeFit;
        }
        void TempRenderer::setRadius(double radius)
        {
            m_radius = radius;
        }
        void TempRenderer::setDepthCalc(int index)
        {
            m_depthCalc = index;
        }
        void TempRenderer::setSmoothNum(int sNum)
        {
            m_smoothNum = sNum;
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
            m_textures[RendererTextures_Depth]->Generate(m_width, m_height, GL_DEPTH_COMPONENT);
            m_textures[RendererTextures_Normal]->Generate(m_width, m_height, GL_RGBA);
            m_textures[RendererTextures_Position]->Generate(m_width, m_height, GL_RGBA);
            m_textures[RendererTextures_ESmooth]->Generate(m_width, m_height, GL_RGBA);
            m_textures[RendererTextures_FitPos]->Generate(m_width, m_height, GL_RGBA);
            m_textures[RendererTextures_Shade]->Generate(m_width, m_height, GL_RGBA);
            m_textures[RendererTextures_Mask]->Generate(m_width, m_height, GL_RGBA);
            m_textures[RendererTextures_OSmooth]->Generate(m_width, m_height, GL_RGBA);
            m_textures[RendererTextures_ThirdColor]->Generate(m_width, m_height, GL_RGBA);
            m_textures[RendererTextures_ThirdData]->Generate(m_width, m_height, GL_RGBA);
            m_textures[RendererTextures_Sing]->Generate(m_width, m_height, GL_RGBA);
            m_textures[RendererTextures_Analysis1]->Generate(m_width, m_height, GL_RGBA);
            m_textures[RendererTextures_Analysis2]->Generate(m_width, m_height, GL_RGBA);
            m_textures[RendererTextures_Analysis3]->Generate(m_width, m_height, GL_RGBA);
            m_textures[RendererTextures_Filter]->Generate(m_width, m_height, GL_RGBA);
            m_textures[RendererTextures_Disp]->Generate(m_width, m_height, GL_RGBA);
            m_textures[RendererTextures_Eye]->Generate(m_width, m_height, GL_RGBA);
            m_textures[RendererTextures_Ks]->Generate(m_width, m_height, GL_RGBA);
            m_textures[RendererTextures_Kd]->Generate(m_width, m_height, GL_RGBA);
            m_textures[RendererTextures_Ns]->Generate(m_width, m_height, GL_RGBA);
            m_textures[RendererTextures_Stroke]->Generate(m_width, m_height, GL_RGBA);

            m_fbo->bind();
            glViewport( 0, 0, m_width, m_height );
            m_fbo->attachTexture(GL_DEPTH_ATTACHMENT , m_textures[RendererTextures_Depth].get()->texture());
            m_fbo->attachTexture(GL_COLOR_ATTACHMENT0, m_textures[RendererTextures_Position].get()->texture());
            m_fbo->attachTexture(GL_COLOR_ATTACHMENT1, m_textures[RendererTextures_Normal].get()->texture());
            m_fbo->attachTexture(GL_COLOR_ATTACHMENT2, m_textures[RendererTextures_Eye].get()->texture());
            m_fbo->attachTexture(GL_COLOR_ATTACHMENT3, m_textures[RendererTextures_Ks].get()->texture());
            m_fbo->attachTexture(GL_COLOR_ATTACHMENT4, m_textures[RendererTextures_Kd].get()->texture());
            m_fbo->attachTexture(GL_COLOR_ATTACHMENT5, m_textures[RendererTextures_Ns].get()->texture());
            if ( m_fbo->checkStatus() != GL_FRAMEBUFFER_COMPLETE )
            {
                LOG( logERROR ) << "FBO Error : " << m_fbo->checkStatus();
            }
            GL_CHECK_ERROR;

            m_maskFbo->bind();
            glViewport( 0, 0, m_width, m_height );
            m_maskFbo->attachTexture(GL_COLOR_ATTACHMENT0, m_textures[RendererTextures_Mask].get()->texture());
            if ( m_fbo->checkStatus() != GL_FRAMEBUFFER_COMPLETE )
            {
                LOG( logERROR ) << "FBO Error : " << m_fbo->checkStatus();
            }
            GL_CHECK_ERROR;

            m_fitFbo->bind();
            glViewport( 0, 0, m_width, m_height );
            m_fitFbo->attachTexture(GL_COLOR_ATTACHMENT0, m_textures[RendererTextures_OSmooth].get()->texture());
            m_fitFbo->attachTexture(GL_COLOR_ATTACHMENT1, m_textures[RendererTextures_FitPos].get()->texture());
            m_fitFbo->attachTexture(GL_COLOR_ATTACHMENT2, m_textures[RendererTextures_Shade].get()->texture());
            if ( m_fbo->checkStatus() != GL_FRAMEBUFFER_COMPLETE )
            {
                LOG( logERROR ) << "FBO Error : " << m_fbo->checkStatus();
            }
            GL_CHECK_ERROR;

            m_evenSmoothFbo->bind();
            glViewport( 0, 0, m_width, m_height );
            m_evenSmoothFbo->attachTexture(GL_COLOR_ATTACHMENT0, m_textures[RendererTextures_ESmooth].get()->texture());
            if ( m_fbo->checkStatus() != GL_FRAMEBUFFER_COMPLETE )
            {
                LOG( logERROR ) << "FBO Error : " << m_fbo->checkStatus();
            }
            GL_CHECK_ERROR;

            m_oddSmoothFbo->bind();
            glViewport( 0, 0, m_width, m_height );
            m_oddSmoothFbo->attachTexture(GL_COLOR_ATTACHMENT0, m_textures[RendererTextures_OSmooth].get()->texture());
            if ( m_fbo->checkStatus() != GL_FRAMEBUFFER_COMPLETE )
            {
                LOG( logERROR ) << "FBO Error : " << m_fbo->checkStatus();
            }
            GL_CHECK_ERROR;

            m_imp3Fbo->bind();
            glViewport( 0, 0, m_width, m_height );
            m_imp3Fbo->attachTexture(GL_COLOR_ATTACHMENT0, m_textures[RendererTextures_ThirdColor].get()->texture());
            m_imp3Fbo->attachTexture(GL_COLOR_ATTACHMENT1, m_textures[RendererTextures_ThirdData].get()->texture());
            if ( m_fbo->checkStatus() != GL_FRAMEBUFFER_COMPLETE )
            {
                LOG( logERROR ) << "FBO Error : " << m_fbo->checkStatus();
            }
            GL_CHECK_ERROR;

            m_singFbo->bind();
            glViewport( 0, 0, m_width, m_height );
            m_singFbo->attachTexture(GL_COLOR_ATTACHMENT0, m_textures[RendererTextures_Sing].get()->texture());
            if ( m_fbo->checkStatus() != GL_FRAMEBUFFER_COMPLETE )
            {
                LOG( logERROR ) << "FBO Error : " << m_fbo->checkStatus();
            }
            GL_CHECK_ERROR;

            m_analysisFbo->bind();
            glViewport( 0, 0, m_width, m_height );
            m_analysisFbo->attachTexture(GL_COLOR_ATTACHMENT0, m_textures[RendererTextures_Analysis1].get()->texture());
            m_analysisFbo->attachTexture(GL_COLOR_ATTACHMENT1, m_textures[RendererTextures_Analysis2].get()->texture());
            m_analysisFbo->attachTexture(GL_COLOR_ATTACHMENT2, m_textures[RendererTextures_Analysis3].get()->texture());
            if ( m_fbo->checkStatus() != GL_FRAMEBUFFER_COMPLETE )
            {
                LOG( logERROR ) << "FBO Error : " << m_fbo->checkStatus();
            }
            GL_CHECK_ERROR;

            m_filterFbo->bind();
            glViewport( 0, 0, m_width, m_height );
            m_filterFbo->attachTexture(GL_COLOR_ATTACHMENT0, m_textures[RendererTextures_Filter].get()->texture());
            if ( m_fbo->checkStatus() != GL_FRAMEBUFFER_COMPLETE )
            {
                LOG( logERROR ) << "FBO Error : " << m_fbo->checkStatus();
            }
            GL_CHECK_ERROR;

            m_dispFbo->bind();
            glViewport( 0, 0, m_width, m_height );
            m_dispFbo->attachTexture(GL_COLOR_ATTACHMENT0, m_textures[RendererTextures_Disp].get()->texture());
            if ( m_fbo->checkStatus() != GL_FRAMEBUFFER_COMPLETE )
            {
                LOG( logERROR ) << "FBO Error : " << m_fbo->checkStatus();
            }
            GL_CHECK_ERROR;

            m_strokeFbo->bind();
            glViewport( 0, 0, m_width, m_height );
            m_strokeFbo->attachTexture(GL_COLOR_ATTACHMENT0, m_textures[RendererTextures_Stroke].get()->texture());
            if ( m_fbo->checkStatus() != GL_FRAMEBUFFER_COMPLETE )
            {
                LOG( logERROR ) << "FBO Error : " << m_fbo->checkStatus();
            }
            GL_CHECK_ERROR;

            // finished with fbo, undbind to bind default
            globjects::Framebuffer::unbind();
            LOG(logDEBUG) << "6";
            uint w = m_width;
            uint h = m_height;
            //delete &texCoords;
            m_mesh.clear();
            texCoords.resize(w*h*4);
            m_mesh.m_vertices.resize(w*h*4);
            m_mesh.m_triangles.resize(w*h*2);
            uint indexCpt = 0;
            int faceInd = 0;
            LOG(logDEBUG) << "7";
            for(int j=0; j<h; ++j){
                for(int i=0; i<w; ++i){
                    Ra::Core::Triangle A = Ra::Core::Triangle(indexCpt, indexCpt+1, indexCpt+2);
                    Ra::Core::Triangle B = Ra::Core::Triangle(indexCpt, indexCpt+2, indexCpt+3);
                    m_mesh.m_triangles[faceInd++] = A;
                    m_mesh.m_triangles[faceInd++] = B;

                    m_mesh.m_vertices[indexCpt] = quadVertices[0];
                    texCoords[indexCpt] = Core::Vector4f(quadTex[0][0], quadTex[0][1], float(i), float(j));
                    indexCpt++;

                    m_mesh.m_vertices[indexCpt] = quadVertices[1];
                    texCoords[indexCpt] = Core::Vector4f(quadTex[1][0], quadTex[1][1], float(i), float(j));
                    indexCpt++;

                    m_mesh.m_vertices[indexCpt] = quadVertices[2];
                    texCoords[indexCpt] = Core::Vector4f(quadTex[2][0], quadTex[2][1], float(i), float(j));
                    indexCpt++;

                    m_mesh.m_vertices[indexCpt] = quadVertices[3];
                    texCoords[indexCpt] = Core::Vector4f(quadTex[3][0], quadTex[3][1], float(i), float(j));
                    indexCpt++;
                }
            }
            LOG(logDEBUG) << "8";
            m_strokeMesh->loadGeometry(m_mesh);
            m_strokeMesh->addData( Ra::Engine::Mesh::VERTEX_COLOR, texCoords );
            m_strokeMesh->updateGL();
            LOG(logDEBUG) << "9";
        }

    }
} // namespace Ra
