#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>

#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/Material.hpp>

namespace Ra {

void Engine::RenderTechnique::updateGL()
{
    if (nullptr == shader)
    {
        shader = ShaderProgramManager::getInstancePtr()->getShaderProgram(shaderConfig);
    }

    if (material)
    {
        material->updateGL();
    }
}

} // namespace Ra
