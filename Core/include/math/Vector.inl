#include "Vector.hpp"
namespace Ra
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
}
