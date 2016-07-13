#ifndef RADIUM_GEOMETRYTESTS_HPP_
#define RADIUM_GEOMETRYTESTS_HPP_

#include <Core/Geometry/Distance/DistanceQueries.hpp>
#include <Core/Math/PolyLine.hpp>

using Ra::Core::DistanceQueries::pointToLineSq;
using Ra::Core::DistanceQueries::pointToSegmentSq;
using Ra::Core::Math::areApproxEqual;
namespace RaTests
{
    class GeometryTests : public Test
    {
        void run() override
        {
            Ra::Core::Vector3 a(1.f, 2.3f, 4.5f);
            Ra::Core::Vector3 b(-6.f, 7.f, 8.9f);

            // Midpoint.
            Ra::Core::Vector3 m = 0.5f*(a+b);

            // Point on the line, before A
            Ra::Core::Vector3 na = a - 12.f*(b-a);

            // Point on the line after B
            Ra::Core::Vector3 nb = a + 42.f*(b-a);


            Ra::Core::Vector3 y,z;
            Ra::Core::Vector::getOrthogonalVectors(b-a, y, z);

            // Test line queries.
            RA_UNIT_TEST(  pointToLineSq(a, a, b-a) == 0.f, "distance from A to AB" );
            RA_UNIT_TEST(  pointToLineSq(b, a, b-a) == 0.f, "distance from B to AB" );
            RA_UNIT_TEST(  areApproxEqual(pointToLineSq(na, a, b-a), 0.f), "point on the line.");
            RA_UNIT_TEST(  areApproxEqual(pointToLineSq(nb, a, b-a), 0.f), "point on the line.");
            RA_UNIT_TEST(  areApproxEqual(pointToLineSq(m + y, a, b-a), y.squaredNorm()), "point perpendicular to segment.");


            // Test segment queries
            RA_UNIT_TEST(  pointToSegmentSq(a, a, b-a) ==  0.f, "Segment extremity" );
            RA_UNIT_TEST(  pointToSegmentSq(b, a, b-a) == 0.f, "Segment extremity" );
            RA_UNIT_TEST(  areApproxEqual(pointToSegmentSq(na, a, b-a) , (na-a).squaredNorm()), "point on the line.");
            RA_UNIT_TEST(  areApproxEqual(pointToSegmentSq(nb, a, b-a) , (nb-b).squaredNorm()), "point on the line.");
            RA_UNIT_TEST(  areApproxEqual(pointToSegmentSq(m + y, a, b-a) , y.squaredNorm()), "point perpendicular to segment.");
        }
    };

    //DECLARE

    class PolylineTests : public Test
    {
        void run() override
        {
            // 2 points polyline
            {
                Ra::Core::Vector3Array v2 { {1,2,3}, {4,5,6}};
                Ra::Core::PolyLine p(v2);

                Ra::Core::Vector3 m = 0.5f * (v2[0] + v2[1]);

                RA_UNIT_TEST(p.f(0) == v2[0], "Parametrization fail");
                RA_UNIT_TEST(p.f(1) == v2[1], "Parametrization fail");
                RA_UNIT_TEST(p.f(0.5f).isApprox(m), "Parametrization fail");
                RA_UNIT_TEST (Ra::Core::Math::areApproxEqual(p.distance(m), 0.f), "Distance fail" );
            }
            // 4 points polyline
            {

                Ra::Core::Vector3Array v4{{2, 3, 5}, {7, 11, 13}, {17, 23, 29}, {-1, -1, 30}};

                Ra::Core::PolyLine p(v4);

                RA_UNIT_TEST(p.f(0) == v4[0], "Parametrization fail");
                RA_UNIT_TEST(p.f(1) == v4[3], "Parametrization fail");
                RA_UNIT_TEST(p.f(-10.) == p.f(0), "Parametrization clamp fail");
                RA_UNIT_TEST(p.f(10.) == p.f(1), "Parametrization clamp fail");

                for (const auto& x : v4)
                {
                    RA_UNIT_TEST(p.distance(x) == 0.f, "Distance fail");
                }
            }
        }
    };

    RA_TEST_CLASS(GeometryTests);
    RA_TEST_CLASS(PolylineTests);
}


#endif //RADIUM_GEOMETRYTESTS_HPP_
