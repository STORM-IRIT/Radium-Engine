#include <Core/Animation/Handle/Handle.hpp>

namespace Ra {
namespace Core {
namespace Animation {

inline uint Handle::size() const {
    return m_pose.size();
}

inline std::string Handle::getName() const {
    return m_name;
}

inline void Handle::setName( const Label& name ) {
    m_name = name;
}

inline Label Handle::getLabel( const uint i ) const {
    CORE_ASSERT( ( i < size() ), "Index i out of bound" );
    return m_label.at( i );
}

inline void Handle::setLabel( const uint i, const Label& text ) {
    CORE_ASSERT( ( i < size() ), "Index i out of bound" );
    m_label[i] = text;
}

} // namespace Animation
} // Namespace Core
} // Namespace Ra
