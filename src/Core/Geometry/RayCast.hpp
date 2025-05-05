#pragma once

#include <Core/Geometry/IndexedGeometry.hpp>
#include <Core/RaCore.hpp>
#include <Core/Types.hpp>
#include <Eigen/Core>
#include <vector>

// useful : http://www.realtimerendering.com/intersections.html

namespace Ra {
namespace Core {
// Low-level intersection functions of line versus various abstract shapes.
// All functions return true if there was a hit, false if not.
// If a ray starts inside the shape, the resulting hit will be at the ray's origin (t=0).

namespace Geometry {

/// Intersect a ray with an axis-aligned bounding box.
bool RA_CORE_API RayCastAabb( const Ray& r,
                              const Core::Aabb& aabb,
                              Scalar& hitOut,
                              Vector3& normalOut );

/// Intersects a ray with a sphere.
bool RA_CORE_API RayCastSphere( const Ray& r,
                                const Core::Vector3& center,
                                Scalar radius,
                                std::vector<Scalar>& hitsOut );

/// Intersect a ray with an infinite plane defined by point A and normal.
bool RA_CORE_API RayCastPlane( const Ray& r,
                               const Core::Vector3& a,
                               const Core::Vector3& normal,
                               std::vector<Scalar>& hitsOut );

/// Intersect  a ray with a cylinder with a and b as caps centers and given radius.
bool RA_CORE_API RayCastCylinder( const Ray& r,
                                  const Core::Vector3& a,
                                  const Core::Vector3& b,
                                  Scalar radius,
                                  std::vector<Scalar>& hitsOut );

/// Intersect a ray with a triangle abc.
bool RA_CORE_API RayCastTriangle( const Ray& r,
                                  const Core::Vector3& a,
                                  const Core::Vector3& b,
                                  const Core::Vector3& c,
                                  std::vector<Scalar>& hitsOut );

bool RA_CORE_API RayCastTriangleMesh( const Ray& r,
                                      const TriangleMesh& mesh,
                                      std::vector<Scalar>& hitsOut,
                                      std::vector<Vector3ui>& trianglesIdxOut );
} // namespace Geometry
} // namespace Core
} // namespace Ra
