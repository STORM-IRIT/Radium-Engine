#ifndef RADIUMENGINE_RAY_CAST_HPP_
#define RADIUMENGINE_RAY_CAST_HPP_

#include <Core/RaCore.hpp>
#include <Core/Types.hpp>
#include <Eigen/Core>
#include <vector>

// useful : http://www.realtimerendering.com/intersections.html

namespace Ra {
namespace Core {
namespace Geometry {

class TriangleMesh;

/** \name Ray casting
 * Low-level intersection functions of line versus various abstract shapes.
 * All functions return true if there was a hit, false otherwise.
 * If a ray starts inside the shape, the resulting hit will be at the ray's origin (t=0).
 */
/// \{

/**
 * Intersect a ray with an axis-aligned bounding box.
 * \p normalOut normal at the hit point.
 */
RA_CORE_API bool RayCastAabb( const Ray& r, const Core::Aabb& aabb, Scalar& hitOut,
                              Vector3& normalOut );

/**
 * Intersect a ray with a sphere.
 */
// FIXME: why vector since in the implementation there can be only one?
RA_CORE_API bool RayCastSphere( const Ray& r, const Core::Vector3& center, Scalar radius,
                                std::vector<Scalar>& hitsOut );

/**
 * Intersect a ray with an infinite plane defined by point A and normal.
 */
// FIXME: why vector since in the implementation there can be only one?
RA_CORE_API bool RayCastPlane( const Ray& r, const Core::Vector3 a, const Core::Vector3& normal,
                               std::vector<Scalar>& hitsOut );

/**
 * Intersect a ray with a cylinder with a and b as caps centers and given radius.
 */
// FIXME: why vector since in the implementation there can be only one?
RA_CORE_API bool RayCastCylinder( const Ray& r, const Core::Vector3& a, const Core::Vector3& b,
                                  Scalar radius, std::vector<Scalar>& hitsOut );

/**
 * Intersect a ray with a triangle abc.
 */
// FIXME: why vector since in the implementation there can be only one?
RA_CORE_API bool RayCastTriangle( const Ray& r, const Core::Vector3 a, const Core::Vector3& b,
                                  const Core::Vector3& c, std::vector<Scalar>& hitsOut );

/**
 * Intersect a ray with a triangle mesh.
 */
// FIXME: why vector of scalars since in the implementation there can be only one?
// FIXME: why vector of triangles since there will be only one hit?
// FIXME: why having htis version and the ont in TriangleMesh?
RA_CORE_API bool RayCastTriangleMesh( const Ray& r, const TriangleMesh& mesh,
                                      std::vector<Scalar>& hitsOut,
                                      std::vector<Vector3ui>& trianglesIdxOut );
/// \}

} // namespace Geometry
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_RAY_CAST_HPP_
