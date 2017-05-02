#include "PolyLine.hpp"
#include <Core/Geometry/PointCloud/PointCloud.hpp>

namespace Ra {
namespace Core {

const Vector3Array& PolyLine::getPoints() const
{
    return m_pts;
}

Scalar PolyLine::length() const
{
    return m_lengths.back();
}

Ra::Core::Aabb PolyLine::aabb() const
{
    return PointCloud::aabb( m_pts );
}

Scalar PolyLine::getLineParameter ( uint segment, Scalar tSegment ) const
{
    CORE_ASSERT( segment < m_ptsDiff.size(), "invalid segment index");
    const Scalar lprev = segment  > 0 ? m_lengths[segment - 1] : 0;
    const Scalar lSegment = m_lengths[segment] - lprev;
    return ((lSegment * tSegment) + lprev) / length();
}

void PolyLine::getSegment(uint segment, Vector3& aOut, Vector3& abOut) const
{
    CORE_ASSERT( segment < m_ptsDiff.size(), "Invalid segment index.");
    aOut = m_pts[segment]; abOut = m_ptsDiff[segment];
}
const Vector3Array& PolyLine::getSegmentVectors() const
{
    return m_ptsDiff;
}
}}
