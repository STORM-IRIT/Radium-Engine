#include <Core/Math/PolyLine.hpp>


namespace Ra {
namespace Core {

void PolyLine::update()
{
    CORE_ASSERT( m_pts.size() > 1, "Line must have at least two points" );
    m_ptsDiff.reserve( m_pts.size() - 1 );
    m_lengths.reserve( m_pts.size() - 1 );
    Scalar len = 0;
    for ( uint i = 0; i < m_pts.size() - 1; ++i )
    {
        m_ptsDiff.push_back( m_pts[i + 1] - m_pts[i] );
        len += m_ptsDiff.back().norm();
        m_lengths.push_back( len );
    }

}

PolyLine::PolyLine( const Vector3Array& pts ) : m_pts( pts )
{
    update();
}

void PolyLine::setPoints( const Vector3Array& pts )
{
    m_pts = pts;
    update();
}

Scalar PolyLine::squaredDistance( const Vector3& p ) const
{
    CORE_ASSERT( m_pts.size() > 1, "Line must have at least two points" );
    Scalar sqDist = std::numeric_limits<Scalar>::max();
    for ( uint i = 0; i < m_ptsDiff.size(); ++i )
    {
        sqDist = std::min( DistanceQueries::pointToSegmentSq( p, m_pts[i], m_ptsDiff[i] ), sqDist );
    }
    return sqDist;
}

Scalar PolyLine::distance( const Vector3& p ) const
{
    return std::sqrt( squaredDistance( p ) );
}

Scalar PolyLine::project( const Vector3& p ) const
{
    CORE_ASSERT( m_pts.size() > 1, "Line must have at least two points" );
    Scalar sqDist = std::numeric_limits<Scalar>::max();
    uint segment = 0;
    Scalar t = 0;
    for ( uint i = 0; i < m_ptsDiff.size(); ++i )
    {
        Scalar proj = DistanceQueries::projectOnSegment( p, m_pts[i], m_ptsDiff[i] );
        Scalar d = (p - (m_pts[i] + proj * (m_ptsDiff[i]))).squaredNorm();
        if ( sqDist < d )
        {
            t = proj;
            segment = i;
        }
    }

    CORE_ASSERT( segment < m_ptsDiff.size(), "Invalid index" );
    Scalar lprev = segment  > 0 ? m_lengths[segment - 1] : 0;
    return (((m_lengths[segment] * t) + lprev) / length());
}

Ra::Core::Vector3 PolyLine::f( Scalar t ) const
{
    // Clamp the parameter between 0 and 1 and scale it.
    const Scalar param = length() * Ra::Core::Math::saturate( t );

    // Try to locate the segment section where f(t) belongs.
    uint i = 0;
    Scalar lprev = 0.f;
    while ( m_lengths[i] < param )
    {
        lprev = m_lengths[i];
        ++i;
    }

    CORE_ASSERT( i < m_ptsDiff.size(), "Invalid index" );

    // Now we know point f(t) is between point Pi and Pi+1;
    return m_pts[i] + ((param - lprev) / (m_lengths[i] - lprev)) * m_ptsDiff[i];
}

}
}

