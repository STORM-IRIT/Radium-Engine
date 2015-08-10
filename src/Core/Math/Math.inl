#include <Core/Math/Math.hpp>

namespace Ra
{
    namespace Core
    {

        inline Scalar Math::toRadians( Scalar a )
        {
            Scalar result = a * Pi / 180.0;
            return result;
        }

        inline Scalar Math::toDegrees( Scalar a )
        {
            Scalar result = a * 180 / Pi;
            return result;
        }

    }
}