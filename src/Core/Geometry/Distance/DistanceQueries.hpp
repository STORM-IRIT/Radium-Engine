#ifndef RADIUMENGINE_DISTANCE_QUERIES_HPP_
#define RADIUMENGINE_DISTANCE_QUERIES_HPP_
#include <Core/RaCore.hpp>
#include <Core/Math/LinearAlgebra.hpp>

namespace Ra
{
    namespace Core
    {
        namespace DistanceQueries
        {
            // Return the squared distance from point Q to the line defined by point A and direction dir.
            inline Scalar pointToLineSq(  const Vector3& q, const Vector3& a, const Vector3& dir )
            {
                // http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html
                return (dir.cross(q-a)).squaredNorm() / dir.squaredNorm();
            }

            // Return the squared distance from point Q to the segment AB defined by point A and
            // vector AB = (B - A).
            inline Scalar pointToSegmentSq( const Vector3& q, const Vector3& a, const Vector3& ab )
            {
                CORE_ASSERT( ab.squaredNorm() > 0, "Segment has 0 length" );
                const Scalar t = Ra::Core::Math::clamp<Scalar>( (q-a).dot(ab)/(ab.squaredNorm()), 0, 1);
                return (q - ( a + t * (ab))).squaredNorm();
            }
        } // ns Distance queries
    }// ns Core
} // ns Ra


#endif //RADIUMENGINE_DISTANCE_QUERIES_HPP_
