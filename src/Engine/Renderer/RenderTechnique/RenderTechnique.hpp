#ifndef RADIUMENGINE_RENDERTECHNIQUE_HPP
#define RADIUMENGINE_RENDERTECHNIQUE_HPP

#include <Engine/RaEngine.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>

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
        // TODO(Val) : we need proper memory management of these objects.
        struct RenderTechnique
        {
            ShaderConfiguration shaderConfig;
            const ShaderProgram* shader = nullptr;
            Material* material = nullptr;

            RA_ENGINE_API void changeShader( const ShaderConfiguration& newConfig );

            RA_ENGINE_API void updateGL();
            bool shaderIsDirty;
        };

    } // namespace Engine
} // namespace Ra


#endif // RADIUMENGINE_RENDERTECHNIQUE_HPP
