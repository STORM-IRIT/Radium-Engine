#ifndef RADIUMENGINE_POLYLINE_HPP_
#define RADIUMENGINE_POLYLINE_HPP_

#include <Core/Containers/VectorArray.hpp>
#include <Core/Geometry/Distance/DistanceQueries.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {

/// A parametrized polyline, i.e. a continuous polygonal chain of segments.
/// Points go from P0 to Pn. The ith segments joins Pi and Pi+1.
class RA_CORE_API PolyLine {

  public:
    /// Create a polyline from a given set of points.
    PolyLine( const Vector3Array& pt );

    /// Get the point vector.
    inline const Vector3Array& getPoints() const;

    /// Get the \p segment-th segment AB as starting point \p aOut and vector \p abOut.
    inline void getSegment( uint segment, Vector3& aOut, Vector3& abOut ) const;

    /// Get the segment vector ( Pi+1 - Pi)
    inline const Vector3Array& getSegmentVectors() const;

    /// Get the aabb of the polyline.
    inline Aabb aabb() const;

    /// Get the total length of the line .
    inline Scalar length() const;

    /// Update the points of the polyline.
    void setPoints( const Vector3Array& pt );

    /// Return the squared distance between the line and the given point \p p.
    Scalar squaredDistance( const Vector3& p ) const;

    /// Return the distance between the line and the given point \p p.
    Scalar distance( const Vector3& p ) const;

    /// Returns the parameter t in [0,1] corresponding to the point on the
    /// \p segment-th segment closest from point \p p.
    Scalar projectOnSegment( const Vector3& p, uint segment ) const;

    /// Returns the index of the nearest segment to \p p.
    uint getNearestSegment( const Vector3& p ) const;

    /// Returns the index of the segment to which \p t belongs.
    inline uint getSegmentIndex( Scalar t ) const;

    /// Returns the parameter t in [0,1] corresponding to the point on the line
    /// which is the closest point from \p p.
    Scalar project( const Vector3& p ) const;

    /// Return a point on the line corresponding to parameter \p t in [0;1].
    /// Values of t below 0 map to the first point, and values above 1 to the last.
    Vector3 f( Scalar t ) const;

  protected:
    /// Update the precomputed values after new points have been set.
    void update();

    /// Given a point on one of the segments identified by parameter \p tSegment in [0,1]
    /// on the segment-th segment, return the t in [0,1] corresponding to the same point
    /// in the whole polyline parametrization.
    inline Scalar getLineParameter( uint segment, Scalar tSegment ) const;

  private:
    /// The polyline points.
    Vector3Array m_pts;

    /// The polyline segments vector.
    Vector3Array m_ptsDiff;

    /// The polyline segments length.
    std::vector<Scalar> m_lengths;
};

} // namespace Core
} // namespace Ra
#include <Core/Math/PolyLine.inl>

#endif // RADIUMENGINE_POLYLINE_HPP_
