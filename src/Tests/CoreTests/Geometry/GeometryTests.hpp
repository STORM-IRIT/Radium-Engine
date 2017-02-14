#ifndef RADIUM_GEOMETRYTESTS_HPP_
#define RADIUM_GEOMETRYTESTS_HPP_

#include <Core/Geometry/Distance/DistanceQueries.hpp>
#include <Core/Math/PolyLine.hpp>

using Ra::Core::DistanceQueries::pointToLineSq;
using Ra::Core::DistanceQueries::pointToSegmentSq;
using Ra::Core::DistanceQueries::pointToTriSq;
using Ra::Core::DistanceQueries::PointToTriangleOutput;
using Ra::Core::DistanceQueries::FlagsInternal;
using Ra::Core::Math::areApproxEqual;
using Ra::Core::Vector3;
namespace RaTests
{
    class GeometryTests : public Test
    {
        void run() override
        {
            Vector3 a(1.f, 2.3f, 4.5f);
            Vector3 b(-6.f, 7.f, 8.9f);
            Vector3 c(-3.f, 12.3, -42.1);

            // Midpoint.
            Vector3 m = 0.5f*(a+b);

            // Point on the line, before A
            Vector3 na = a - 12.f*(b-a);

            // Point on the line after B
            Vector3 nb = a + 42.f*(b-a);


            Vector3 y,z;
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


            // Test triangle queries

            // Test that each vertex returns itself

            auto da = pointToTriSq(a,a,b,c);
            RA_UNIT_TEST( da.distanceSquared == 0.f, "distance from A to ABC" );
            RA_UNIT_TEST( da.meshPoint == a, "distance from A to ABC");
            RA_UNIT_TEST( da.flags == FlagsInternal::HIT_A , "distance from A to ABC");

            auto db = pointToTriSq(b,a,b,c);
            RA_UNIT_TEST( db.distanceSquared == 0.f, "distance from B to ABC" );
            RA_UNIT_TEST( db.meshPoint == b, "distance from B to ABC");
            RA_UNIT_TEST( db.flags == FlagsInternal::HIT_B, "distance from B to ABC");

            auto dc = pointToTriSq(c,a,b,c);
            RA_UNIT_TEST( dc.distanceSquared == 0.f, "distance from C to ABC" );
            RA_UNIT_TEST( dc.meshPoint == c, "distance from C to ABC");
            RA_UNIT_TEST( dc.flags == FlagsInternal::HIT_C, "distance from C to ABC");

            // Test midpoints of edges
            Vector3 mab = 0.5f * (a+b);
            Vector3 mac = 0.5f * (a+c);
            Vector3 mbc = 0.5f * (b+c);

            auto dmab = pointToTriSq(mab, a,b,c);
            RA_UNIT_TEST( areApproxEqual(dmab.distanceSquared, 0.f), "Distance from AB midpoint to ABC");
            RA_UNIT_TEST( dmab.meshPoint.isApprox(mab), "Distance from AB midpoint to ABC");
            RA_UNIT_TEST( dmab.flags == FlagsInternal::HIT_AB, "Distance from AB midpoint to ABC");

            auto dmac = pointToTriSq(mac, a,b,c);
            RA_UNIT_TEST( areApproxEqual(dmac.distanceSquared, 0.f), "Distance from AC midpoint to ABC");
            RA_UNIT_TEST( dmac.meshPoint.isApprox(mac), "Distance from AC midpoint to ABC");
            RA_UNIT_TEST( dmac.flags == FlagsInternal::HIT_CA, "Distance from AC midpoint to ABC");

            auto dmbc = pointToTriSq(mbc, a,b,c);
            RA_UNIT_TEST( areApproxEqual(dmbc.distanceSquared, 0.f), "Distance from BC midpoint to ABC");
            RA_UNIT_TEST( dmbc.meshPoint.isApprox(mbc), "Distance from BC midpoint to ACC");
            RA_UNIT_TEST( dmbc.flags == FlagsInternal::HIT_BC, "Distance from BC midpoint to ACC");

            // Point inside the triangle
            Vector3 g = (1.f/3.f) * (a+b+c);

            auto dg = pointToTriSq(g,a,b,c);
            RA_UNIT_TEST( areApproxEqual(dg.distanceSquared, 0.f), "Distance from centroid to ABC");
            RA_UNIT_TEST( dg.meshPoint.isApprox(g), "Distance from centroid to ABC");
            RA_UNIT_TEST( dg.flags == FlagsInternal::HIT_FACE, "Distance from centroid to ABC");
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
