#ifndef RADIUMENGINE_MATH_HPP
#define RADIUMENGINE_MATH_HPP

#include <Core/CoreMacros.hpp>

#ifdef OS_WINDOWS
#define _USE_MATH_DEFINES
#include <cmath>
#else
#include <cmath>
#endif 

namespace Ra { namespace Core { 

    namespace Math
    {

	constexpr Scalar Pi     = Scalar(3.141592653589793);
	constexpr Scalar InvPi  = Scalar(1.0 / Pi);
	constexpr Scalar PiDiv2 = Scalar(Pi / 2.0);
	constexpr Scalar PiDiv4 = Scalar(Pi / 4.0);
	constexpr Scalar PiMul2 = Scalar(Pi * 2.0);
	constexpr Scalar toRad  = Scalar(Pi / 180.0);
	constexpr Scalar toDeg  = Scalar(180 * InvPi);

    } // namespace Math
}
}

#endif // RADIUMENGINE_MATH_HPP