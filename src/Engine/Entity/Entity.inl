#include <Engine/Entity/Entity.hpp>

namespace Ra
{

    inline const std::string& Engine::Entity::getName() const
    {
        return m_name;
    }

    inline void Engine::Entity::rename ( const std::string& name )
    {
        m_name = name;
    }

    inline void Engine::Entity::setTransform ( const Core::Transform& transform )
    {
        std::lock_guard<std::mutex> lock ( m_mutex );
        m_transform = transform;
    }

    inline void Engine::Entity::setTransform ( const Core::Matrix4& transform )
    {
        std::lock_guard<std::mutex> lock ( m_mutex );
        m_transform = Core::Transform ( transform );
    }

    inline Core::Transform Engine::Entity::getTransform() const
    {
        std::lock_guard<std::mutex> lock ( m_mutex );
        return m_transform;
    }

    inline Core::Matrix4 Engine::Entity::getTransformAsMatrix() const
    {
        std::lock_guard<std::mutex> lock ( m_mutex );
        return m_transform.matrix();
    }

} // namespace Ra
