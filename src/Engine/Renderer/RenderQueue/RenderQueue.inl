#include <Engine/Renderer/RenderQueue/RenderQueue.hpp>

namespace Ra
{

template <typename Key, typename Child>
inline Engine::RenderableMap<Key, Child>::RenderableMap()
    : std::map<Key, Child>()
{
}

template <typename Key, typename Child>
inline Engine::RenderableMap<Key, Child>::~RenderableMap()
{
}

template <typename Key, typename Child>
inline void Engine::RenderableMap<Key, Child>::render(ShaderProgram* shader) const
{
    for (auto& it : *this)
    {
        it.first.bind(shader);
        it.second.render(shader);
    }
}

inline Engine::MeshVector::MeshVector()
    : std::vector<Mesh*>()
{
}

inline Engine::MeshVector::~MeshVector()
{
}

inline void Engine::MeshVector::render(ShaderProgram *shader) const
{
    CORE_UNUSED(shader);
    for (auto& it : *this)
    {
        it->render();
    }
}

inline Engine::RenderQueue::RenderQueue()
    : std::map<ShaderKey, MaterialRenderQueue>()
{
}

inline Engine::RenderQueue::~RenderQueue()
{
}

inline void Engine::RenderQueue::render() const
{
    for (auto& it : *this)
    {
        it.first.bind();
        it.second.render(it.first.getShader());
    }
}

inline void Engine::RenderQueue::render(const Engine::RenderParameters& params) const
{
    for (auto& it : *this)
    {
        it.first.bind(params);
        it.second.render(it.first.getShader());
    }
}

inline void Engine::RenderQueue::render(Engine::ShaderProgram* shader) const
{
    ShaderKey key(shader);
    key.bind();

    for (auto& it : *this)
    {
        it.second.render(key.getShader());
    }
}

inline void Engine::RenderQueue::render(Engine::ShaderProgram* shader, const RenderParameters& params) const
{
    ShaderKey key(shader);
    key.bind(params);

    for (auto& it : *this)
    {
        it.second.render(key.getShader());
    }
}

} // namespace Ra
