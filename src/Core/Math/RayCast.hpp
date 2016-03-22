#ifndef RADIUMENGINE_RAY_CAST_HPP_
#define RADIUMENGINE_RAY_CAST_HPP_

#include <Core/RaCore.hpp>
#include <vector>
#include <Core/Math/Ray.hpp>
#include <Core/Mesh/TriangleMesh.hpp>

// useful : http://www.realtimerendering.com/intersections.html

namespace Ra
{
    namespace Core
    {
        /// Raycast function versus various abstract shapes.
        /// * All functions return true if there was at least a valid (t>=0) hit, false if the ray misses.
        /// * If a ray starts inside the shape, the resulting hit will be at the ray's origin (t=0).
        /// * Some functions may return several hits. If the result is true,
        /// the first hit will always be the first positive hit.

        namespace RayCast
        {
            /// Intersect a ray with an axis-aligned bounding box.
            inline bool vsAabb(const Ray& r, const Core::Aabb& aabb, std::vector<Scalar>& hitsOut);

            /// Intersects a ray with a sphere.
            inline bool vsSphere(const Ray& r, const Core::Vector3& center, Scalar radius, std::vector<Scalar>& hitsOut);

            /// Intersect a ray with an infinite plane defined by point A and normal.
            inline bool vsPlane(const Ray& r, const Core::Vector3 a, const Core::Vector3& normal, std::vector<Scalar>& hitsOut);

            /// Intersect  a ray with a cylinder with a and b as caps centers and given radius.
            inline bool vsCylinder(const Ray& r, const Core::Vector3& a, const Core::Vector3& b, Scalar radius,
                            std::vector<Scalar>& hitsOut);

            inline bool vsTriangle( const Ray& r, const Core::Vector3 a, const Core::Vector3& b, const Core::Vector3& c,
                            std::vector<Scalar>& hitsOut);

            // FIXME(Charly): Not efficient, intersecting against a kd-tree would be ways faster.
            inline bool vsTriangleMesh(const Ray& r, const TriangleMesh& mesh, std::vector<Scalar>& hitsOut, std::vector<Triangle>& trianglesIdxOut);
        }
    }
}
#include <Core/Math/RayCast.inl>

#endif // RADIUMENGINE_RAY_CAST_HPP_

