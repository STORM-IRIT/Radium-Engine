#ifndef RADIUMENGINE_MAPPING_DEFINITION
#define RADIUMENGINE_MAPPING_DEFINITION

#include <Core/Containers/AlignedStdVector.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Index/Index.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

class Mapping {
public:
    /// CONSTRUCTOR
    inline Mapping( const Scalar alpha = 1.0,
                    const Scalar beta  = 0.0,
                    const Scalar delta = 0.0,
                    const Index& id    = Index::INVALID_IDX() );

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
    inline bool isFinite() const;
    inline bool isPositive() const;
    inline bool isBoundToElement() const;

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
