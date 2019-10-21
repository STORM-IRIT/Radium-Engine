#include <Core/Animation/HandleSystem.hpp>

namespace Ra {
namespace Core {
namespace Animation {

inline uint HandleSystem::size() const {
    return m_pose.size();
}

inline std::string HandleSystem::getName() const {
    return m_name;
}

inline void HandleSystem::setName( const Label& name ) {
    m_name = name;
}

inline Label HandleSystem::getLabel( const uint i ) const {
    CORE_ASSERT( ( i < size() ), "Index i out of bound" );
    return m_label.at( i );
}

inline void HandleSystem::setLabel( const uint i, const Label& text ) {
    CORE_ASSERT( ( i < size() ), "Index i out of bound" );
    m_label[i] = text;
}

} // namespace Animation
} // Namespace Core
} // Namespace Ra
