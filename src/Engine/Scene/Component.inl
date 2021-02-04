#pragma once
#include <Engine/Scene/Component.hpp>

namespace Ra {
namespace Engine {
namespace Scene {

inline void Component::setEntity( Entity* entity ) {
    m_entity = entity;
}

inline Entity* Component::getEntity() const {
    return m_entity;
}

inline const std::string& Component::getName() const {
    return m_name;
}

inline void Component::setSystem( System* system ) {
    m_system = system;
}

inline System* Component::getSystem() const {
    return m_system;
}

} // namespace Scene
} // namespace Engine
} // namespace Ra
