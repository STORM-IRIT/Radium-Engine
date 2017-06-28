#include <Engine/Renderer/Renderers/ExperimentalRenderer.hpp>

#include <iostream>

#include <Core/Log/Log.hpp>
#include <Core/Math/ColorPresets.hpp>
#include <Core/Containers/Algorithm.hpp>
#include <Core/Containers/MakeShared.hpp>

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

        ExperimentalRenderer::ExperimentalRenderer( uint width, uint height )
            : Renderer(width, height)
        {
            LOG(logINFO) << "Building an  ExperimentalRenderer ";
        }

        ExperimentalRenderer::~ExperimentalRenderer()
        {
            LOG(logINFO) << "Deleting an  ExperimentalRenderer ";
        }

        void ExperimentalRenderer::initializeInternal()        
        {
            LOG(logINFO) << "ExperimentalRenderer::initializeInternal ";
        }

        void ExperimentalRenderer::initShaders()
        {
            LOG(logINFO) << "ExperimentalRenderer::initShaders ";
        }

        void ExperimentalRenderer::initBuffers()
        {
            LOG(logINFO) << "ExperimentalRenderer::initBuffers ";
        }

        void ExperimentalRenderer::updateStepInternal( const RenderData& renderData )
        {
            LOG(logINFO) << "ExperimentalRenderer::updateStepInternal ";
        }

        void ExperimentalRenderer::renderInternal( const RenderData& renderData )
        {
            LOG(logINFO) << "ExperimentalRenderer::renderInternal ";
        }

        // Draw debug stuff, do not overwrite depth map but do depth testing
        void ExperimentalRenderer::debugInternal( const RenderData& renderData )
        {
            LOG(logINFO) << "ExperimentalRenderer::debugInternal ";
        }

        // Draw UI stuff, always drawn on top of everything else + clear ZMask
        void ExperimentalRenderer::uiInternal( const RenderData& renderData )
        {
            LOG(logINFO) << "ExperimentalRenderer::uiInternal ";
        }

        void ExperimentalRenderer::postProcessInternal( const RenderData& renderData )
        {
            LOG(logINFO) << "ExperimentalRenderer::postProcessInternal ";
        }

        void ExperimentalRenderer::resizeInternal()
        {
            LOG(logINFO) << "ExperimentalRenderer::resizeInternal ";
        }

    }
} // namespace Ra
