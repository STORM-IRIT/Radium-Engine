#ifndef RADIUMENGINE_RAY_CAST_HPP_
#define RADIUMENGINE_RAY_CAST_HPP_

#include <Core/RaCore.hpp>
#include <Core/Math/Ray.hpp>

namespace Ra
{
    namespace Core
    {
        namespace RayCast
        {
            bool vsAabb( const Ray& r, const Core::Aabb& aabb, std::vector<Scalar>& hitsOut)
            {

                return false;
            }


            bool vsSphere(const Ray& r, const Core::Vector3& center, Scalar radius, std::vector<Scalar>& hitsOut)
            {

                CORE_ASSERT(ray.m_direction.squaredNorm() > 0.f, "Invalid Ray");

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
                    const Scalar t1 = (-b - std::sqrt(delta)) / (2.f*a);
                    const Scalar t2 = (-b + std::sqrt(delta)) / (2.f*a);

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

