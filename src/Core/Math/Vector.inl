#include "Vector.hpp"

namespace Ra { namespace Core
{
    template<typename Vector_>
    inline Vector_ Vector::floor(const Vector_& v)
    {
        typedef typename Vector_::Scalar Scalar_;
        Vector_ result(v);
        result.unaryExpr(std::function<Scalar_(Scalar_)>(static_cast<Scalar_(&)(Scalar_)>(std::floor)));
        return result;
    }

    template<typename Vector_>
    inline Vector_ Vector::ceil(const Vector_& v)
    {
        typedef typename Vector_::Scalar Scalar_;
        Vector_ result(v);
        result.unaryExpr(std::function<Scalar_(Scalar_)>(static_cast<Scalar_(&)(Scalar_)>(std::ceil)));
        return result;
    }

    template<typename Vector_>
    inline Vector_ Vector::clamp(const Vector_& v, const Vector_& min, const Vector_& max)
    {
        Vector_ result(v);
        result.cwiseMin(max);
        result.cwiseMax(min);
        return result;
    }

    Matrix3 Math::computeRotation(const Vector3& a, const Vector3& b, const Vector3& defaultAxis)
    {
        // http://math.stackexchange.com/questions/180418/calculate-rotation-matrix-to-align-vector-a-to-vector-b-in-3d/476311#476311
        CORE_ASSERT( (a.norm() - 1.f < 0.001f)
                  && (b.norm() - 1.f < 0.001f) 
                  , "Expected unit vectors");

        const Vector3 v = a.cross(b);
        const Scalar sin2 = v.squaredNorm(); // sinus of the angle (a,b) squared

        const Scalar cos = a.dot(b);
        // TODO (Val) : can we optimize this without branches ?
        if( LIKELY(sin2 != 0.f))
        {
            // M is the skew-symetric cross product by v matrix
            // Matrix M such as M*x = v.cross(x)
            // For proof of the formula see
            // https://en.wikipedia.org/wiki/Cross_product#Conversion_to_matrix_multiplication
            const Matrix3 M = (b * a.transpose()) - (a * b.transpose());
            const Matrix3 R = Matrix3::Identity() + M + ((1 - cos) / (sin2) * (M * M));

            return R;
        }
        else // Degenerate case where either a = b or a = -b
        {
            if(  a == b )
            {
                return Matrix3::Identity();
            }
            else
            {
                return AngleAxis(Scalar(M_PI), defaultAxis).toRotationMatrix();
            }
        }
    }

    Quaternion operator+ (const Quaternion& q1, const Quaternion& q2)
    {
        return Quaternion(q1.coeffs()+q2.coeffs());
    }

    Quaternion operator* (const Scalar& k, const Quaternion& q)
    {
        return Quaternion(k* q.coeffs());
    }


}}
