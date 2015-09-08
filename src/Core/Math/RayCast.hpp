#ifndef RADIUMENGINE_RAY_CAST_HPP_
#define RADIUMENGINE_RAY_CAST_HPP_

#include <Core/RaCore.hpp>
#include <Core/Math/Ray.hpp>

// useful : http://www.realtimerendering.com/intersections.html

namespace Ra
{
    namespace Core
    {
        namespace RayCast
        {
            bool vsAabb( const Ray& r, const Core::Aabb& aabb, std::vector<Scalar>& hitsOut)
            {
                // Based on optimized Woo version
                // Ref : Graphics Gems p.395
                // http://www.codercorner.com/RayAABB.cpp

                CORE_ASSERT(r.m_direction.squaredNorm() > 0.f, "Invalid Ray");
                CORE_ASSERT(!aabb.isEmpty(), "Empty AABB"); // Or return false ?

                // Vector of bool telling which components of the direction are not 0;
                auto nEqualZero = r.m_direction.array() != Core::Vector3::Zero().array();

                // Vector of bool telling which components of the ray origin are respectively
                // smaller than the aabb min or higher than aabb max.
                auto infMin = r.m_origin.array() < aabb.min().array();
                auto supMax = r.m_origin.array() > aabb.max().array();

                // Get rid of the case where the origin of the ray is inside the box
                if ( !(infMin.any()) && !(supMax.any()) )
                {
                    hitsOut.push_back(0);
                    return true;
                }

                /// Precompute the t values for each plane.
                Core::Vector3 invDir = r.m_direction.cwiseInverse();
                Core::Vector3 minOrig = (aabb.min() - r.m_origin).cwiseProduct(invDir);
                Core::Vector3 maxOrig = (aabb.max() - r.m_origin).cwiseProduct(invDir);

                // The following Eigen dark magic should be equivalent to this pseudo code
                // For  i = 1..3
                // if (direction[i] !=0)
                //    if (origin[i] < aabb.min) then maxT[i] = (min[i] - origin[i]) / direction[i])
                //    else if (origin[i] > aabb.max) then maxT[i] = (max[i] - origin[i]) / direction[i]

                auto maxT =
                nEqualZero.select(
                    infMin.select(
                            minOrig.array(),
                            supMax.select(
                                    maxOrig.array(),
                                    -1.f
                            )
                    ),
                    -1.f
                );

                uint maxTIdx;
                Scalar t = maxT.maxCoeff(&maxTIdx);

                // Early out for negative t (box behind the origin)
                if (t < 0) { return false; }

                Core::Vector3 point = r.at(t);
                if (aabb.contains(point))
                {
                    hitsOut.push_back(t);
                    return true;
                }
                return false;
            }

            bool vsSphere(const Ray& r, const Core::Vector3& center, Scalar radius, std::vector<Scalar>& hitsOut)
            {

                CORE_ASSERT(r.m_direction.squaredNorm() > 0.f, "Invalid Ray");

                // Solve a 2nd degree eqn. in t
                // X = ray.origin + t* ray.direction
                // ||X - center|| = radius.

                const Core::Vector3 co = r.m_origin -  center;
                const Scalar co2 = co.squaredNorm();
                const Scalar dirDotCO = r.m_direction.dot(co);

                // t is one solution of at^2 + bt + c = 0;
                const Scalar c = co2 - (radius * radius);
                const Scalar b =  2.f * dirDotCO;
                const Scalar a = r.m_direction.squaredNorm();

                const Scalar delta = (b * b) - ( 4.f * a *c);

                if (delta == 0.f)
                {
                    const Scalar t =  - b / (2.f * a);
                    const bool tPositive = (t >= 0.f);
                    if (tPositive) {hitsOut.push_back(t);}
                    return tPositive;
                }
                else if (delta > 0.f)
                {
                    const Scalar div = 1.f / (2.f*a);
                    const Scalar t1 = (-b - std::sqrt(delta)) * div;
                    const Scalar t2 = (-b + std::sqrt(delta)) * div;

                    // We know this because a is > 0;
                    CORE_ASSERT(t1 < t2, "Your math is wrong.");

                    const bool t1Positive = (t1 >= 0.f);
                    const bool t2Positive = (t2 >= 0.f);
                    if (t1Positive) {hitsOut.push_back(t1);}
                    if (t2Positive) {hitsOut.push_back(t2);}

                    return (t1Positive || t2Positive);
                }
                return false;
            }
        }
    }
}


#endif // RADIUMENGINE_RAY_CAST_HPP_

