#ifndef RADIUMENGINE_MATH_HPP
#define RADIUMENGINE_MATH_HPP

#include <Core/RaCore.hpp>

#include <algorithm>
#include <cmath>
namespace Ra {
namespace Core {
namespace Math {
/// Mathematical constants casted to Scalar. Values taken from math.h
constexpr Scalar Sqrt2 = Scalar( 1.41421356237309504880 );  // sqrt(2)
constexpr Scalar e = Scalar( 2.7182818284590452354 );       // e = exp(1).
constexpr Scalar Pi = Scalar( 3.14159265358979323846 );     // pi
constexpr Scalar InvPi = Scalar( 0.31830988618379067154 );  // 1/pi
constexpr Scalar PiDiv2 = Scalar( 1.57079632679489661923 ); // pi/2
constexpr Scalar PiDiv3 = Scalar( 1.04719755119659774615 ); // pi/3
constexpr Scalar PiDiv4 = Scalar( 0.78539816339744830962 ); // pi/4
constexpr Scalar PiDiv6 = Scalar( 0.52359877559829887307 ); // pi/6
constexpr Scalar PiMul2 = Scalar( 2 * Pi );                 // 2*pi
constexpr Scalar toRad = Scalar( Pi / 180.0 );
constexpr Scalar toDeg = Scalar( 180.0 * InvPi );

constexpr Scalar machineEps = std::numeric_limits<Scalar>::epsilon();
constexpr Scalar dummyEps = Scalar( 1e-5 );

/// Useful functions

/// Converts an angle from degrees to radians.
inline constexpr Scalar toRadians( Scalar a );

/// Converts an angle from radians to degrees.
inline constexpr Scalar toDegrees( Scalar a );

/// Returns true if |a -b| < eps.
inline bool areApproxEqual( Scalar a, Scalar b, Scalar eps = dummyEps );

/// Integer power functions. Work for all numeric types which support
/// multiplication and for which T(1) is a valid expression.
/// x^0 always return T(1) and x^1 always return x (even when x is 0).

/// Run-time exponent version.
template <typename T>
inline T ipow( const T& x, uint exp );

/// Compile-time exponent version.
template <uint N, typename T>
inline constexpr T ipow( const T& x );

/// Returns the sign of any numeric type as { -1, 0, 1}
template <typename T>
inline constexpr int sign( const T& val );

/// Returns the sign of any numeric type as { -1, 1}
/// Note: signNZ(0) returns 1 for any integral type
/// but signNZ(-0.f) will return -1
template <typename T>
inline constexpr T signNZ( const T& val );

/// Returns value v clamped between bounds min and max.
template <typename T>
inline constexpr T clamp( T v, T min, T max );

/// Clamps the value between 0 and 1
template <typename T>
inline constexpr T saturate( T v );

/// Returns the linear interpolation between a and b
template <typename T>
inline constexpr T lerp( const T& a, const T& b, Scalar t );

} // namespace Math
} // namespace Core
} // namespace Ra

#include <Core/Math/Math.inl>

#endif // RADIUMENGINE_MATH_HPP
