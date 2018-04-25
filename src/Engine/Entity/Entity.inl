#include <Engine/Entity/Entity.hpp>

namespace Ra {
namespace Engine {

inline const std::string& Entity::getName() const {
    return m_name;
}

inline void Entity::rename( const std::string& name ) {
    m_name = name;
}

inline void Entity::setTransform( const Core::Math::Transform& transform ) {
    m_transformChanged = true;
    m_doubleBufferedTransform = transform;
}

inline void Entity::setTransform( const Core::Math::Matrix4& transform ) {
    setTransform( Core::Math::Transform( transform ) );
}

inline Core::Math::Transform Entity::getTransform() const {
    std::lock_guard<std::mutex> lock( m_transformMutex );
    return m_transform;
}

inline Core::Math::Matrix4 Entity::getTransformAsMatrix() const {
    std::lock_guard<std::mutex> lock( m_transformMutex );
    return m_transform.matrix();
}

inline uint Entity::getNumComponents() const {
    return uint( m_components.size() );
}
} // namespace Engine

} // namespace Ra
