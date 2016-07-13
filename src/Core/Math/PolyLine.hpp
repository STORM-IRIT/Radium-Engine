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
        /// A parametrized polyline.
        class PolyLine
        {

        public:
            PolyLine(const Vector3Array& pts)
                    : m_pts(pts)
            {
                update();
            }

            const Vector3Array& getPoints() const { return m_pts; }

            void setPoints(const Vector3Array& pts)
            {
                m_pts = pts;
                update();
            }

            /// return the squared distance between the line and point p.
            Scalar squaredDistance(const Vector3& p) const
            {
                CORE_ASSERT(m_pts.size() > 1, "Line must have at least two points");
                Scalar sqDist = std::numeric_limits<Scalar>::max();
                for (uint i = 0; i < m_ptsDiff.size(); ++i)
                {
                    sqDist = std::min(DistanceQueries::pointToSegmentSq(p, m_pts[i], m_ptsDiff[i]), sqDist);
                }
                return sqDist;
            }

            Scalar distance(const Vector3& p) const
            {
                return std::sqrt(squaredDistance(p));
            }

            inline Scalar length() const
            {
                return m_lengths.back();
            }

            Vector3 f(Scalar t)
            {
                // Clamp the parameter between 0 and 1 and scale it.
                const Scalar param = length() * Ra::Core::Math::clamp(t, 0.f, 1.f);

                // Try to locate the segment section where f(t) belongs.
                uint i = 0;
                Scalar lprev = 0.f;
                while (m_lengths[i] < param)
                {
                    lprev = m_lengths[i];
                    ++i;
                }

                CORE_ASSERT(  i < m_ptsDiff.size(), "Invalid index");

                // Now we know point f(t) is between point Pi and Pi+1;
                return m_pts[i] + ((param - lprev) / (m_lengths[i] - lprev)) * m_ptsDiff[i];
            }

        protected:
            void update()
            {
                CORE_ASSERT(m_pts.size() > 1, "Line must have at least two points");
                m_ptsDiff.reserve(m_pts.size() - 1);
                m_lengths.reserve(m_pts.size() - 1);
                Scalar len = 0;
                for (uint i = 0; i < m_pts.size() -1; ++i)
                {
                    m_ptsDiff.push_back(m_pts[i + 1] - m_pts[i]);
                    len += m_ptsDiff.back().norm();
                    m_lengths.push_back(len);
                }
            }

        private:
            // Stores the points Pi
            Vector3Array m_pts;
            // Stores the vectors (Pi+1 - Pi)
            Vector3Array m_ptsDiff;

            // Length from origin to point Pi+1
            std::vector<Scalar> m_lengths;
        };

    }
}

#endif //RADIUMENGINE_POLYLINE_HPP_
