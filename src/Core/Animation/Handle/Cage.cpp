#include <Core/Animation/Handle/Cage.hpp>

namespace Ra {
namespace Core {
namespace Animation {

Cage::Cage() : Handle(), m_triangle() {}
Cage::Cage( const uint n ) : Handle( n ), m_triangle() {}
Cage::Cage( const Cage& cage ) : Handle( cage ), m_triangle( cage.m_triangle ) {}

Cage::~Cage() {}

void Cage::clear() {
    m_pose.clear();
    m_triangle.clear();
}

} // namespace Animation
} // Namespace Core
} // Namespace Ra
