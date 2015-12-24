#ifndef RADIUMENGINE_RENDERQUEUE_HPP
#define RADIUMENGINE_RENDERQUEUE_HPP

#include <Engine/RaEngine.hpp>

#include <map>
#include <vector>

#include <Core/Containers/AlignedStdVector.hpp>

#include <Engine/Renderer/RenderQueue/ShaderKey.hpp>
#include <Engine/Renderer/Bindable/BindableMaterial.hpp>
#include <Engine/Renderer/Bindable/BindableTransform.hpp>
#include <Engine/Renderer/Bindable/BindableMesh.hpp>


namespace Ra
{
    namespace Engine
    {
        class ShaderProgram;
        class RenderParameters;
    }
}

namespace Ra
{
    namespace Engine
    {
        class RenderQueue : public Core::AlignedStdVector<BindableMesh>
        {
        public:
            inline RA_ENGINE_API RenderQueue();
            inline virtual RA_ENGINE_API ~RenderQueue() {}

            inline RA_ENGINE_API void render() const;
            inline RA_ENGINE_API void render( const RenderParameters& params ) const;
            inline RA_ENGINE_API void render( ShaderProgram* shader ) const;
            inline RA_ENGINE_API void render( ShaderProgram* shader, const RenderParameters& params ) const;
        };
    } // namespace Engine
} // namespace Ra

#include <Engine/Renderer/RenderQueue/RenderQueue.inl>

#endif // RADIUMENGINE_RENDERQUEUE_HPP
