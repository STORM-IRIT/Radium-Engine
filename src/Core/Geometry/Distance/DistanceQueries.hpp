#ifndef RADIUMENGINE_DISTANCE_QUERIES_HPP_
#define RADIUMENGINE_DISTANCE_QUERIES_HPP_

#include <Core/RaCore.hpp>
#include <Core/Math/LinearAlgebra.hpp>

/// Functions in this file are utilities to compute the distance between various geometric sets.
/// They always return the squared distance.


namespace Ra
{
    namespace Core
    {
        namespace DistanceQueries
        {
            //
            // Point-to-line distance
            //


            /// Return the squared distance from point Q to the line defined by point A and direction dir.
            inline RA_CORE_API Scalar pointToLineSq(const Vector3& q, const Vector3& a, const Vector3& dir);

            /// Project point Q on segment AB defined by point A and vector AB = (B -A).
            /// Return the parameter t in [0,1] which identifies the projected point.
            inline RA_CORE_API Scalar projectOnSegment(const Vector3& q, const Vector3& a, const Vector3& ab);

            /// Return the squared distance from point Q to the segment AB defined by point A and
            /// vector AB = (B - A).
            inline RA_CORE_API Scalar pointToSegmentSq(const Vector3& q, const Vector3& a, const Vector3& ab);

            //
            // Point-to-triangle distance
            //

            /// Returns the squared distance d from a query point Q to the triangle ABC.
            /// triPoint T is computed as the closest point on the triangle.
            /// Thus ||QT|| = d.
            /// Triangle ABC must not be degenerate.
            inline RA_CORE_API Scalar pointToTriSq(const Vector3& q, const Vector3& a, const Vector3& b, const Vector3& c,
                                                   Vector3& triPoint);


        } // ns Distance queries
    }// ns Core
} // ns Ra

#include <Core/Geometry/Distance/DistanceQueries.inl>

#endif //RADIUMENGINE_DISTANCE_QUERIES_HPP_
