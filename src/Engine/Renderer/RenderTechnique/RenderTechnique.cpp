#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>

#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/Material.hpp>

namespace Ra
{
    void Engine::RenderTechnique::changeShader( const ShaderConfiguration &newConfig )
    {
        shaderConfig = newConfig;
        shaderIsDirty = true;
    }

    void Engine::RenderTechnique::updateGL()
    {
        if ( nullptr == shader || shaderIsDirty )
        {
            shader = ShaderProgramManager::getInstance()->getShaderProgram(shaderConfig);
            shaderIsDirty = false;
        }

        if ( material )
        {
            material->updateGL();
        }
    }

} // namespace Ra
