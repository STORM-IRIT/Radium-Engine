#ifndef RADIUMENGINE_RENDERTECHNIQUE_HPP
#define RADIUMENGINE_RENDERTECHNIQUE_HPP

#include <Engine/RaEngine.hpp>
#include <memory>
#include <Engine/Renderer/RenderTechnique/ShaderConfiguration.hpp>

namespace Ra
{
    namespace Engine
    {
        class ShaderProgram;
        class Material;
    }
}

namespace Ra
{
    namespace Engine
    {
        // NOTE(Charly): Would it be interesting to provide more stuff here ?
        struct RenderTechnique
        {
            ShaderConfiguration shaderConfig;
            const ShaderProgram* shader = nullptr;
            std::shared_ptr<Material> material = nullptr;

            RA_ENGINE_API void changeShader( const ShaderConfiguration& newConfig );

            RA_ENGINE_API void updateGL();
            bool shaderIsDirty;

            RA_ENGINE_API ShaderConfiguration getBasicConfiguration() const;

        };

    } // namespace Engine
} // namespace Ra


#endif // RADIUMENGINE_RENDERTECHNIQUE_HPP
