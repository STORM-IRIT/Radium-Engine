#pragma once

#include <Core/Containers/VectorArray.hpp>
#include <Core/Geometry/DistanceQueries.hpp>
#include <Core/RaCore.hpp>
#include <Core/Types.hpp>

namespace Ra {
namespace Core {
namespace Geometry {
/// A parametrized polyline, i.e. a continuous polygonal chain of segments.
/// Points go from P0 to Pn. The ith segments joins Pi and Pi+1.
class RA_CORE_API PolyLine
{

  public:
    /// Create a polyline from a given set of points.
    explicit PolyLine( const Vector3Array& pt );

    /// Get the point vector.
    inline const Vector3Array& getPoints() const;

    /// Get the ith segment AB as starting point A and vector AB.
    inline void getSegment( uint segment, Vector3& aOut, Vector3& abOut ) const;

    /// Get the segment vector ( Pi+1 - Pi)
    inline const Vector3Array& getSegmentVectors() const;

    /// Get the aabb of the polyline.
    inline Aabb aabb() const;

    /// Get the total length of the line .
    inline Scalar length() const;

    /// Update the points of the polyline.
    void setPoints( const Vector3Array& pt );

    /// Return the squared distance between the line and a given point p.
    Scalar squaredDistance( const Vector3& p ) const;

    /// Return the distance between the line and a given point p.
    Scalar distance( const Vector3& p ) const;

    /// Returns the parameter t in [0,1] corresponding to the point on the
    /// ith segment closest from point p.
    Scalar projectOnSegment( const Vector3& p, uint segment ) const;

    /// Returns the index of the nearest segment.
    uint getNearestSegment( const Vector3& p ) const;

    /// Returns the index of the segment to which t belons
    inline uint getSegmentIndex( Scalar t ) const;

    /// Returns the parameter t in [0,1] corresponding to the point on the line
    /// which is the closest point from p.
    Scalar project( const Vector3& p ) const;

    /// Return a point on the line corresponding to parameter t in [0;1].
    /// Values of t below 0 map to the first point, and values above 1 to the last.
    Vector3 f( Scalar t ) const;

  protected:
    /// Update the precomputed values after new points have been set.
    void update();

    /// Given a point on one of the segments identified by parameter tSegment in [0,1]
    /// and segment number, return the t in [0,1] corresponding to the same point
    /// in the whole line parametrization.
    inline Scalar getLineParameter( uint segment, Scalar tSegment ) const;

  private:
    // Stores the points Pi
    Vector3Array m_pts;
    // Stores the vectors (Pi+1 - Pi)
    Vector3Array m_ptsDiff;
    // Length from origin to point Pi+1.
    std::vector<Scalar> m_lengths;
};

} // namespace Geometry
} // namespace Core
} // namespace Ra

#include <Core/Math/LinearAlgebra.hpp> // cotan, saturate (from Math.hpp)

namespace Ra {
namespace Core {
namespace Geometry {

const Vector3Array& PolyLine::getPoints() const {
    return m_pts;
}

Scalar PolyLine::length() const {
    return m_lengths.back();
}

Aabb PolyLine::aabb() const {
    Aabb aabb;
    for ( const auto& v : m_pts ) {
        aabb.extend( v );
    }
    return aabb;
}

Scalar PolyLine::getLineParameter( uint segment, Scalar tSegment ) const {
    CORE_ASSERT( segment < m_ptsDiff.size(), "invalid segment index" );
    const Scalar lprev    = segment > 0 ? m_lengths[segment - 1] : 0;
    const Scalar lSegment = m_lengths[segment] - lprev;
    return ( ( lSegment * tSegment ) + lprev ) / length();
}

void PolyLine::getSegment( uint segment, Vector3& aOut, Vector3& abOut ) const {
    CORE_ASSERT( segment < m_ptsDiff.size(), "Invalid segment index." );
    aOut  = m_pts[segment];
    abOut = m_ptsDiff[segment];
}

const Vector3Array& PolyLine::getSegmentVectors() const {
    return m_ptsDiff;
}

uint PolyLine::getSegmentIndex( Scalar t ) const {
    // This could be optimized by dichotomy
    Scalar param = length() * Math::saturate( t );
    uint i       = 0;
    while ( m_lengths[i] < param ) {
        ++i;
    }
    return i;
}

} // namespace Geometry
} // namespace Core
} // namespace Ra
