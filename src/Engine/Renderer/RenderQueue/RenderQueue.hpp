#ifndef RADIUMENGINE_RENDERQUEUE_HPP
#define RADIUMENGINE_RENDERQUEUE_HPP

#include <map>
#include <vector>

#include <Core/CoreMacros.hpp>
#include <Engine/Renderer/Shader/ShaderProgram.hpp>
#include <Engine/Renderer/RenderQueue/RenderParameters.hpp>
#include <Engine/Renderer/RenderQueue/ShaderKey.hpp>
#include <Engine/Renderer/RenderQueue/BindableMaterial.hpp>
#include <Engine/Renderer/RenderQueue/BindableTransform.hpp>
#include <Engine/Renderer/RenderQueue/BindableMesh.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>


namespace Ra { namespace Engine {

template <typename Key, typename Child>
class RA_API RenderableMap : public std::map<Key, Child>
{
public:
    inline RenderableMap();
    inline virtual ~RenderableMap();

    inline void render(ShaderProgram* shader) const;
};

class RA_API MeshVector : public std::vector<Mesh*>
{
public:
    inline MeshVector();
    inline virtual ~MeshVector();

    inline void render(ShaderProgram* shader) const;
};

typedef RenderableMap<BindableTransform, MeshVector> TransformRenderQueue;
typedef RenderableMap<BindableMaterial, TransformRenderQueue> MaterialRenderQueue;

class RA_API RenderQueue : public std::map<ShaderKey, MaterialRenderQueue>
{
public:
    inline RenderQueue();
    inline virtual ~RenderQueue();

    inline void render() const;
    inline void render(const Engine::RenderParameters& params) const;
    inline void render(Engine::ShaderProgram* shader) const;
    inline void render(ShaderProgram* shader, const RenderParameters& params) const;
};

} // namespace Engine
} // namespace Ra

#include <Engine/Renderer/RenderQueue/RenderQueue.inl>

#endif // RADIUMENGINE_RENDERQUEUE_HPP
