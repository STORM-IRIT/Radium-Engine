#ifndef RADIUMENGINE_RENDERTECHNIQUE_HPP
#define RADIUMENGINE_RENDERTECHNIQUE_HPP

#include <Core/CoreMacros.hpp>
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
        // TODO(Val) : we need proper memory management of these objects.
        struct RenderTechnique
        {
            ShaderConfiguration shaderConfig;
            ShaderProgram* shader = nullptr;
            Material* material = nullptr;

            RA_API void updateGL();
        };

    } // namespace Engine
} // namespace Ra


#endif // RADIUMENGINE_RENDERTECHNIQUE_HPP
