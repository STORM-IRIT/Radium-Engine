#pragma once
#include <Core/Random/RandomPointSet.hpp>

namespace Ra {
namespace Core {
namespace Random {

template <class PointSet>
Ra::Core::Vector3 SphericalPointSet<PointSet>::projectOnSphere( const Ra::Core::Vector2&& pt ) {
    Scalar theta = std::acos( 2 * pt[1] - 1 ); // 0 <= tetha <= pi
    Scalar phi   = 2_ra * Scalar( M_PI ) * pt[0];
    return { std::sin( theta ) * std::cos( phi ),
             std::sin( theta ) * std::sin( phi ),
             std::cos( theta ) };
}

template <class PointSet>
SphericalPointSet<PointSet>::SphericalPointSet( size_t n ) : p( n ) {}

template <class PointSet>
size_t SphericalPointSet<PointSet>::range() {
    return p.range();
}

template <class PointSet>
Ra::Core::Vector3 SphericalPointSet<PointSet>::operator()( size_t i ) {
    return projectOnSphere( p( i ) );
}

} // namespace Random
} // namespace Core
} // namespace Ra
