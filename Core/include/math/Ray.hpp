#ifndef RADIUMENGINE_RAY_HPP
#define RADIUMENGINE_RAY_HPP

#include <math/Vector.hpp>

namespace Ra
{
    /// A structure representing a ray in space with an origin and a direction.
    // TODO : use Eigen parametrized line ?
    struct Ray
    {
    public:
        /// Default constructor initializes the position and direction to zero.
        Ray() : m_origin(Vector3::Zero()), m_direction(Vector3::Zero()) { }

        /// Construct a ray with given position and direction.
        Ray(const Vector3& origin, const Vector3& direction) : m_origin(origin), m_direction(direction) { }

        /// Retunrs the position at linear coordinate t on the ray.
        inline Vector3 at(Scalar t) const
        {
            return m_origin + (t * m_direction);
        }

    public:
        /// Starting point of the ray
        Vector3 m_origin;
        /// Direction of the ray (not necessarily normalized).
        Vector3 m_direction;
    };

}

#endif //RADIUMENGINE_RAY_HPP
