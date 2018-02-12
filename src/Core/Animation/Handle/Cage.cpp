#include <Core/Animation/Handle/Cage.hpp>


namespace Ra {
namespace Core {
namespace Animation {

/// CONSTRUCTOR
Cage::Cage() : PointCloud(), m_triangle() { }
Cage::Cage( const uint n ) : PointCloud( n ), m_triangle() { }
Cage::Cage( const Cage& cage ) : PointCloud( cage ), m_triangle( cage.m_triangle ) { }

/// DESTRUCTOR
Cage::~Cage() { }

/// SIZE
void Cage::clear() {
    m_pose.clear();
    m_triangle.clear();
}

} // namespace Animation
} // Namespace Core
} // Namespace Ra


