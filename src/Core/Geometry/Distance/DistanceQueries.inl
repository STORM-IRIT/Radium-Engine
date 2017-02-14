#include <Core/Mesh/MeshUtils.hpp>
#include "DistanceQueries.hpp"

namespace Ra
{
    namespace Core
    {
        namespace DistanceQueries
        {
            // Line funcs

            inline RA_CORE_API Scalar pointToLineSq(const Vector3& q, const Vector3& a, const Vector3& dir)
            {
                // http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html
                return (dir.cross(q - a)).squaredNorm() / dir.squaredNorm();
            }

            inline RA_CORE_API Scalar projectOnSegment(const Vector3& q, const Vector3& a, const Vector3& ab)
            {
                // Edge case : segment has length 0
                if (UNLIKELY(ab.squaredNorm() == 0)) { return 0; }
                return Ra::Core::Math::clamp<Scalar>((q - a).dot(ab) / (ab.squaredNorm()), 0, 1);
            }

            inline RA_CORE_API Scalar pointToSegmentSq(const Vector3& q, const Vector3& a, const Vector3& ab)
            {
                const Scalar t = projectOnSegment(q, a, ab);
                return (q - (a + t * (ab))).squaredNorm();
            }

            // Triangle funcs


            enum FlagsInternal
            {
                HIT_FACE = PointToTriangleOutput::HIT_FACE,

                HIT_A = 0x0 | PointToTriangleOutput::HIT_VERTEX,
                HIT_B = 0x4 | PointToTriangleOutput::HIT_VERTEX,
                HIT_C = 0x8 | PointToTriangleOutput::HIT_VERTEX,

                HIT_AB = 0x0 | PointToTriangleOutput::HIT_EDGE,
                HIT_BC = 0x4 | PointToTriangleOutput::HIT_EDGE,
                HIT_CA = 0x8 | PointToTriangleOutput::HIT_EDGE,
            };

            inline RA_CORE_API PointToTriangleOutput pointToTriSq(const Vector3& q, const Vector3& a, const Vector3& b, const Vector3& c)
            {
                /*
                 *  This function projects the query point Q on the triangle plane to solve the
                 *  planar problem described by the following schema  of Voronoi zones :
                 * 3     /
                 * --  C
                 *     |\\     6
                 *     |  \\
                 * 5   |    \\     /
                 *     |      \\  /
                 * --  A ------ B    2
                 *     |        |
                 *  1  |   4    |
                 *
                 * It first checks if the nearest point is a vertex ( zones 1,2,3), then if
                 * it belongs to an edge ( zone, 4,5,6). If not the nearest point lies on the
                 * triangle.
                 * Reference : Fast Distance Queries for Triangles, Lines and Points using SSE instructions
                 * Evan Shellshear, Robin Ytterlid.
                 * http://jcgt.org/published/0003/04/05/paper.pdf
                 * Journal of Computer Graphics Techniques, Vol.3 No. 4 (2014)
                 */

                PointToTriangleOutput output;

                const Vector3 ab = b - a;
                const Vector3 ac = c - a;
                const Vector3 aq = q - a;

                CORE_ASSERT(ab.cross(ac).squaredNorm() > 0, "Triangle ABC is degenerate");

                const Scalar d1 = ab.dot(aq);
                const Scalar d2 = ac.dot(aq);

                // Closest point is A. (zone 1)
                const bool m1 = d1 <= 0 && d2 <= 0;
                if (m1)
                {
                    output.meshPoint = a;
                    output.distanceSquared =  aq.squaredNorm();
                    output.flags = FlagsInternal::HIT_A;
                    return output;
                }


                const Vector3 bq = q - b;
                const Scalar d3 = ab.dot(bq);
                const Scalar d4 = ac.dot(bq);

                // Closest point is B (zone 2)
                const bool m2 = d3 >= 0 && d4 <= d3;
                if (m2)
                {
                    output.meshPoint = b;
                    output.distanceSquared = bq.squaredNorm();
                    output.flags = FlagsInternal::HIT_B;
                    return output;
                }

                const Vector3 cq = q - c;
                const Scalar d5 = ab.dot(cq);
                const Scalar d6 = ac.dot(cq);

                // Closest point is C (zone 3)
                const bool m3 = (d6 >= 0 && d5 <= d6);
                if (m3)
                {
                    output.meshPoint = c;
                    output.distanceSquared = cq.squaredNorm();
                    output.flags = FlagsInternal::HIT_C;
                    return output;
                }

                const Scalar vc = d1 * d4 - d3 * d2;
                const Scalar v1 = d1 / (d1 - d3);

                // Closest point is on AB (zone 4)
                const bool m4 = vc <= 0 && d1 >= 0 && d3 <= 0;
                if (m4)
                {
                    output.meshPoint = a + v1 * ab;
                    output.distanceSquared = (output.meshPoint - q).squaredNorm();
                    output.flags = FlagsInternal::HIT_AB;
                    return output;
                }

                const Scalar vb = d5 * d2 - d1 * d6;
                const Scalar w1 = d2 / (d2 - d6);

                // Closest point is on AC (zone 5)
                const bool m5 = vb <= 0 && d2 >= 0 && d6 <= 0;
                if (m5)
                {
                    output.meshPoint = a + w1 * ac;
                    output.distanceSquared = (output.meshPoint - q).squaredNorm();
                    output.flags = FlagsInternal::HIT_CA;
                    return output;
                }

                const Scalar va = d3 * d6 - d5 * d4;
                const Scalar w2 = (d4 - d3) / ((d4 - d3) + (d5 - d6));

                // Closest point is on BC (zone 6)
                const bool m6 = va <= 0 && (d4 - d3) >= 0 && (d5 - d6) >= 0;
                if (m6)
                {
                    output.meshPoint = b + w2 * (c - b);
                    output.distanceSquared = (output.meshPoint - q).squaredNorm();
                    output.flags = FlagsInternal::HIT_BC;
                    return output;
                }


                // Closest point is on the triangle
                const Scalar d = Scalar(1) / (va + vb + vc);
                const Scalar v2 = vb * d;
                const Scalar w3 = vc * d;

                output.meshPoint = a + v2 * ab + w3 * ac;
                output.distanceSquared = (output.meshPoint - q).squaredNorm();
                output.flags = 0;
                return output;
            }

        } // ns Distance queries
    }// ns Core
} // ns Ra

