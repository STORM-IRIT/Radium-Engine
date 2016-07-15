#ifndef RADIUMENGINE_POLYLINE_HPP_
#define RADIUMENGINE_POLYLINE_HPP_

#include <Core/RaCore.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/Geometry/Distance/DistanceQueries.hpp>

namespace Ra
{
    namespace Core
    {
        /// A parametrized polyline, i.e. a continuous polygonal chain of segments.
        class RA_CORE_API PolyLine
        {

        public:
            /// Create a polyline from a given set of points.
            PolyLine(const Vector3Array& pt);


            /// Get the segment extremities.
            inline const Vector3Array& getPoints() const;

            inline Aabb aabb() const;

            /// Get the total length of the line ( constant time).
            inline Scalar length() const;

            /// Update the points of the polyline.
            void setPoints(const Vector3Array& pt);

            /// Return the squared distance between the line and a given point p.
            Scalar squaredDistance(const Vector3& p) const;

            /// Return the distance between the line and a given point p.
            Scalar distance(const Vector3& p) const;
            
            /// Returns a point on the line which is the closes point from p.
            Scalar project( const Vector3& p )const;

            /// Return a point on the line corresponding to parameter t in [0;1].
            /// Values of t below 0 map to the first point, and values above 1 to the last.
            Vector3 f(Scalar t) const ;

        protected:
            /// Update the precomputed values after new points have been set.
            void update();

        private:
            // Stores the points Pi 
            Vector3Array m_pts;
            // Stores the vectors (Pi+1 - Pi)
            Vector3Array m_ptsDiff;
            // Length from origin to point Pi+1.
            std::vector<Scalar> m_lengths;
        };

    }
}
#include <Core/Math/PolyLine.inl>

#endif //RADIUMENGINE_POLYLINE_HPP_
