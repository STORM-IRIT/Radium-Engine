#ifndef RADIUMENGINE_MAPPING_DEFINITION
#define RADIUMENGINE_MAPPING_DEFINITION

#include <Core/Containers/AlignedStdVector.hpp>
#include <Core/Index/Index.hpp>
#include <Core/Math/LinearAlgebra.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

/**
 * The class Mapping stores the mapping of a vertex on a triangle face.
 * It stores the barycentric coordinates of a vertex relative to a triangle,
 * the displacement along a direction from the mapped point on the triangle
 * and the index of the triangle.
 */
class Mapping {
  public:
    inline Mapping( const Scalar alpha = 0.0, const Scalar beta = 0.0, const Scalar delta = 0.0,
                    Index id = Index::Invalid() );

    inline ~Mapping();

    /// Return the first barycentric coordinate.
    inline Scalar getAlpha() const;

    /// Set the first barycentric coordinate.
    inline void setAlpha( const Scalar alpha );

    /// Return the second barycentric coordinate.
    inline Scalar getBeta() const;

    /// Set the second barycentric coordinate.
    inline void setBeta( const Scalar beta );

    /// Return the third barycentric coordinate.
    inline Scalar getGamma() const;

    /// Set the third barycentric coordinate.
    inline Vector3 getCoord() const;

    /// Return the displacement along the direction from the mapped point.
    inline Scalar getDelta() const;

    /// Set the displacement along the direction from the mapped point.
    inline void setDelta( const Scalar delta );

    /// Return the index of the triangle.
    inline Index getID() const;

    /// Set the index of the triangle.
    inline void setID( const Index& id );

    /// Return the point resulting from the mapping to triangle (\p p0, \p p1, \p p2) with
    /// the direction \p n.
    inline Vector3 getPoint( const Vector3& p0, const Vector3& p1, const Vector3& p2,
                             const Vector3& n ) const;

    /// Return true if all the values stored are not nans or infs.
    inline bool isFinite() const;

    /// Return true if the barycentric coordinates are inside the triangle.
    inline bool isInside() const;

    /// Return true if the index of the triangle is valid.
    inline bool isBoundToElement() const;

  protected:
    /// First and second barycentric coordinates for the mapping.
    Vector2 m_coord;

    /// Displacement along any direction fr the mapping.
    Scalar m_delta;

    /// The index of the triangle.
    Index m_id;
};

using Parametrization = AlignedStdVector<Mapping>;

} // namespace Geometry
} // namespace Core
} // namespace Ra

#include <Core/Geometry/Mapping/Mapping.inl>

#endif // RADIUMENGINE_MAPPING_DEFINITION
