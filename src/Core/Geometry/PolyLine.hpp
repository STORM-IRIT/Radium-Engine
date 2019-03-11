#ifndef RADIUMENGINE_POLYLINE_HPP_
#define RADIUMENGINE_POLYLINE_HPP_

#include <Core/Containers/VectorArray.hpp>
#include <Core/Geometry/DistanceQueries.hpp>
#include <Core/Math/Types.hpp>
#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

/**
 * A parametrized polyline, i.e.\ a continuous polygonal chain of segments.
 * Points go from P0 to Pn. The i-th segments joins Pi and Pi+1.
 */
class RA_CORE_API PolyLine {

  public:
    /**
     * Create a polyline from a given set of points.
     */
    PolyLine( const Vector3Array& pt );

    /**
     * Return the polyline points.
     */
    inline const Vector3Array& getPoints() const;

    /**
     * Return the \p segment -th segment AB as starting point \p aOut and vector
     * \p abOut.
     */
    inline void getSegment( uint segment, Vector3& aOut, Vector3& abOut ) const;

    /**
     * Return the polyline's segment vectors.
     */
    inline const Vector3Array& getSegmentVectors() const;

    /**
     * Return the aabb of the polyline.
     */
    inline Aabb aabb() const;

    /**
     * Return the total length of the line.
     */
    inline Scalar length() const;

    /**
     * Set the points of the Polyline and update internal data.
     */
    void setPoints( const Vector3Array& pt );

    /**
     * Return the squared distance between the Polyline and point \p p.
     */
    Scalar squaredDistance( const Vector3& p ) const;

    /**
     * Return the distance between the Polyline and point \p p.
     */
    Scalar distance( const Vector3& p ) const;

    /**
     * Returns the parameter t in [0,1] corresponding to the point on the
     * \p segment -th segment closest from point \p p.
     */
    Scalar projectOnSegment( const Vector3& p, uint segment ) const;

    /**
     * Returns the index of the nearest segment to point \p p.
     */
    uint getNearestSegment( const Vector3& p ) const;

    /**
     * Returns the index of the segment to which curvilinear parameter \p t belongs.
     */
    inline uint getSegmentIndex( Scalar t ) const;

    /**
     * Returns the parameter t in [0,1] corresponding to the point on the Polyline
     * which is the closest from point \p p.
     */
    Scalar project( const Vector3& p ) const;

    /**
     * Return the point on the Polyline corresponding to parameter \p t, in [0;1].
     * Values of t below 0 map to the first point, and values above 1 to the last.
     */
    Vector3 f( Scalar t ) const;

  protected:
    /**
     * Update the precomputed values after new points have been set.
     */
    void update();

    /**
     * Given the \p segment -th segment number and curvilinear parameter \p tSegment
     * in [0,1], returns the curvilinear parameter in [0,1] corresponding to the
     * same point in the whole Polyline parametrization.
     */
    inline Scalar getLineParameter( uint segment, Scalar tSegment ) const;

  private:
    /// Stores the points Pi.
    Vector3Array m_pts;

    /// Stores the vectors (Pi+1 - Pi).
    Vector3Array m_ptsDiff;

    /// Length from origin to point Pi+1.
    std::vector<Scalar> m_lengths;
};

} // namespace Geometry
} // namespace Core
} // namespace Ra
#include <Core/Geometry/PolyLine.inl>

#endif // RADIUMENGINE_POLYLINE_HPP_
