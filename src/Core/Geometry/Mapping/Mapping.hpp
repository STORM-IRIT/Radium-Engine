#ifndef RADIUMENGINE_MAPPING_DEFINITION
#define RADIUMENGINE_MAPPING_DEFINITION

#include <Core/Containers/AlignedStdVector.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Index/Index.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

/*
* The class Mapping stores the mapping of a vertex on a triangle face.
* It stores the followings:
*       ( alpha, beta, gamma ) are the barycentric coordinates of a vertex relative to a triangle. NOTE: gamma = 1 - alpha - beta.
*       delta is the displacement along a direction from the point given by the barycentric coordinates.
*       ID is the index of the triangle.
*/
class Mapping {
public:
    /// CONSTRUCTOR
    inline Mapping( const Scalar alpha = 0.0,
                    const Scalar beta  = 0.0,
                    const Scalar delta = 0.0,
                    Index        id    = Index::Invalid() );

    /// DESTRUCTOR
    inline ~Mapping();

    /// BARYCENTRIC COORDINATE
    inline Scalar getAlpha() const;
    inline void   setAlpha( const Scalar alpha );
    inline Scalar getBeta() const;
    inline void   setBeta( const Scalar beta );
    inline Scalar getGamma() const;
    inline Vector3 getCoord() const;

    /// DELTA
    inline Scalar getDelta() const;
    inline void   setDelta( const Scalar delta );

    /// ID
    inline Index getID() const;
    inline void  setID( const Index& id );

    /// POINT
    inline Vector3 getPoint( const Vector3& p0,
                             const Vector3& p1,
                             const Vector3& p2,
                             const Vector3& n ) const;

    /// QUERY
    inline bool isFinite() const;           // Return true if all the values stored are not nans or infs.
    inline bool isInside() const;         // Return true if the barycentric coordinates are inside the triangle.
    inline bool isBoundToElement() const;   // Return true if the index of the triangle is valid.

protected:
    /// VARIABLE
    Vector2 m_coord;
    Scalar  m_delta;
    Index   m_id;
};

typedef AlignedStdVector< Mapping > Parametrization;

} // namespace Geometry
} // namespace Core
} // namespace Ra

#include <Core/Geometry/Mapping/Mapping.inl>

#endif // RADIUMENGINE_MAPPING_DEFINITION
