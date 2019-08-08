#include <Engine/Component/Component.hpp>

namespace Ra::Engine {
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

} // namespace Ra::Engine
