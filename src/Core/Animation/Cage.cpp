#include <Core/Animation/Cage.hpp>

namespace Ra {
namespace Core {
namespace Animation {

Cage::Cage() : HandleArray(), m_triangle() {}
Cage::Cage( const uint n ) : HandleArray( n ), m_triangle() {}
Cage::Cage( const Cage& cage ) : HandleArray( cage ), m_triangle( cage.m_triangle ) {}

Cage::~Cage() {}

void Cage::clear() {
    m_pose.clear();
    m_triangle.clear();
}

} // namespace Animation
} // Namespace Core
} // Namespace Ra
