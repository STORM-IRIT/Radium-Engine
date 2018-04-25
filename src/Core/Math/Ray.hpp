#ifndef RADIUMENGINE_RAY_HPP
#define RADIUMENGINE_RAY_HPP

#include <Core/Math/LinearAlgebra.hpp>
#include <Core/RaCore.hpp>
#include <Eigen/Geometry>

namespace Ra {
namespace Core {
namespace Math {


/// A structure representing a ray in space with an origin and a direction.
using Ray = Eigen::ParametrizedLine<Scalar, 3>;
inline Ray transformRay( const Ray& r, const Transform& t ) {
    return Ray( t * r.origin(), t.linear() * r.direction() );
}
} // namespace Math
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_RAY_HPP
