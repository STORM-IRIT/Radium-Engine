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
        /// Low-level intersection functions of line versus various abstract shapes.
        /// All functions return true if there was a hit, false if not.
        /// If a ray starts inside the shape, the resulting hit will be at the ray's origin (t=0).

        namespace RayCast
        {

            /// Intersect a ray with an axis-aligned bounding box.
            inline bool vsAabb( const Ray& r, const Core::Aabb& aabb, Scalar& hitOut, Vector3& normalOut );

            /// Intersects a ray with a sphere.
            inline bool vsSphere( const Ray& r, const Core::Vector3& center, Scalar radius, std::vector<Scalar>& hitsOut );

            /// Intersect a ray with an infinite plane defined by point A and normal.
            inline bool vsPlane(const Ray& r, const Core::Vector3 a, const Core::Vector3& normal, std::vector<Scalar>& hitsOut);

            /// Intersect  a ray with a cylinder with a and b as caps centers and given radius.
            inline bool vsCylinder(const Ray& r, const Core::Vector3& a, const Core::Vector3& b, Scalar radius,
                            std::vector<Scalar>& hitsOut);

            /// Intersect a ray with a triangle abc.
            inline bool vsTriangle( const Ray& r, const Core::Vector3 a, const Core::Vector3& b, const Core::Vector3& c,
                            std::vector<Scalar>& hitsOut);

            // FIXME(Charly): Not efficient, intersecting against a kd-tree would be ways faster.
            inline bool vsTriangleMesh(const Ray& r, const TriangleMesh& mesh, std::vector<Scalar>& hitsOut, std::vector<Triangle>& trianglesIdxOut);
        }
    }
}
#include <Core/Math/RayCast.inl>

#endif // RADIUMENGINE_RAY_CAST_HPP_

