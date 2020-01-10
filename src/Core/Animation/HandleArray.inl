#include <Core/Animation/HandleArray.hpp>

namespace Ra {
namespace Core {
namespace Animation {

inline uint HandleArray::size() const {
    return m_pose.size();
}

inline std::string HandleArray::getName() const {
    return m_name;
}

inline void HandleArray::setName( const Label& name ) {
    m_name = name;
}

inline Label HandleArray::getLabel( const uint i ) const {
    CORE_ASSERT( ( i < size() ), "Index i out of bound" );
    return m_label.at( i );
}

inline void HandleArray::setLabel( const uint i, const Label& text ) {
    CORE_ASSERT( ( i < size() ), "Index i out of bound" );
    m_label[i] = text;
}

} // namespace Animation
} // Namespace Core
} // Namespace Ra
