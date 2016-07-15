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

}}
