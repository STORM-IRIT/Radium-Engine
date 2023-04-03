#pragma once

#include <Core/RaCore.hpp>

#include <algorithm>
#include <cmath>
namespace Ra {
namespace Core {

/// This namespace contains usefull math functions.
namespace Math {
/// Mathematical constants casted to Scalar. Values taken from math.h
constexpr Scalar Sqrt2  = Scalar( 1.41421356237309504880 ); // sqrt(2)
constexpr Scalar e      = Scalar( 2.7182818284590452354 );  // e = exp(1).
constexpr Scalar Pi     = Scalar( 3.14159265358979323846 ); // pi
constexpr Scalar InvPi  = Scalar( 0.31830988618379067154 ); // 1/pi
constexpr Scalar PiDiv2 = Scalar( 1.57079632679489661923 ); // pi/2
constexpr Scalar PiDiv3 = Scalar( 1.04719755119659774615 ); // pi/3
constexpr Scalar PiDiv4 = Scalar( 0.78539816339744830962 ); // pi/4
constexpr Scalar PiDiv6 = Scalar( 0.52359877559829887307 ); // pi/6
constexpr Scalar PiMul2 = Scalar( 2 * Pi );                 // 2*pi
constexpr Scalar toRad  = Scalar( Pi / Scalar( 180.0 ) );
constexpr Scalar toDeg  = Scalar( Scalar( 180.0 ) * InvPi );

constexpr Scalar machineEps = std::numeric_limits<Scalar>::epsilon();

/// Useful functions

/// Converts an angle from degrees to radians.
inline constexpr Scalar toRadians( Scalar a );

/// Converts an angle from radians to degrees.
inline constexpr Scalar toDegrees( Scalar a );

/// Compare two numbers such that |x-y| < espilon*epsilonBoostFactor
template <class T>
inline typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
areApproxEqual( T x, T y, T espilonBoostFactor = T( 10 ) );

/// Scalar or component-wise range check.
/// Note: a definition of clamp must be available:
///  - for vectors, relies on the component-wise clamp defined in Core/Math/LinearAlgebra
///  - for scalars, use std version (call `using std::clamp;` beforehand)
template <typename Vector_Or_Scalar>
inline bool checkRange( const Vector_Or_Scalar& v, const Scalar& min, const Scalar& max );

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

/// Clamps the value between 0 and 1
template <typename T>
inline constexpr T saturate( T v );

/// Returns the linear interpolation between a and b
template <typename T>
inline constexpr T lerp( const T& a, const T& b, Scalar t );

/// As define by https://registry.khronos.org/OpenGL-Refpages/gl4/html/smoothstep.xhtml
template <typename T>
T smoothstep( T edge0, T edge1, T x ) {
    using std::clamp;
    T t = clamp( ( x - edge0 ) / ( edge1 - edge0 ), static_cast<T>( 0 ), static_cast<T>( 1 ) );
    return t * t * ( 3.0 - 2.0 * t );
}

template <typename T, template <typename, int...> typename M, int... p>
M<T, p...> smoothstep( T edge0, T edge1, M<T, p...> v ) {
    return v.unaryExpr(
        [edge0, edge1]( T x ) { return Ra::Core::Math::smoothstep( edge0, edge1, x ); } );
}
} // namespace Math
} // namespace Core
} // namespace Ra

namespace Ra {
namespace Core {
namespace Math {

inline constexpr Scalar toRadians( Scalar a ) {
    return toRad * a;
}

inline constexpr Scalar toDegrees( Scalar a ) {
    return toDeg * a;
}

template <typename Vector_Or_Scalar>
inline bool checkRange( const Vector_Or_Scalar& v, const Scalar& min, const Scalar& max ) {
    using std::clamp; // by default, use clamp from the std library
    return clamp( v, min, max ) == v;
}

template <typename T>
inline T ipow( const T& x, uint exp ) {
    if ( exp == 0 ) { return T( 1 ); }
    if ( exp == 1 ) { return x; }
    T p = ipow( x, exp / 2 );
    if ( ( exp % 2 ) == 0 ) { return p * p; }
    else {
        return p * p * x;
    }
}

/// This helper class is needed because C++ doesn't support function template
/// partial specialization.
namespace {
template <typename T, uint N>
struct IpowHelper {
    static inline constexpr T pow( const T& x ) {
        return ( N % 2 == 0 ) ? IpowHelper<T, N / 2>::pow( x ) * IpowHelper<T, N / 2>::pow( x )
                              : IpowHelper<T, N / 2>::pow( x ) * IpowHelper<T, N / 2>::pow( x ) * x;
    }
};

template <typename T>
struct IpowHelper<T, 1> {
    static inline constexpr T pow( const T& x ) { return x; }
};

template <typename T>
struct IpowHelper<T, 0> {
    static inline constexpr T pow( const T& /*x*/ ) { return T( 1 ); }
};

} // namespace

// Nb : T is last for automatic template argument deduction.
template <uint N, typename T>
inline constexpr T ipow( const T& x ) {
    return IpowHelper<T, N>::pow( x );
}

// Signum implementation that works for unsigned types.
template <typename T>
inline constexpr int signum( T x, std::false_type /*is_signed*/ ) {
    return T( 0 ) < x;
}

template <typename T>
inline constexpr int signum( T x, std::true_type /*is_signed*/ ) {
    return ( T( 0 ) < x ) - ( x < T( 0 ) );
}

template <typename T>
inline constexpr int sign( const T& val ) {
    return signum( val, std::is_signed<T>() );
}

template <typename T>
inline constexpr T signNZ( const T& val ) {
    return T( std::copysign( T( 1 ), val ) );
}

template <typename T>
inline constexpr T saturate( T v ) {
    return std::clamp( v, static_cast<T>( 0 ), static_cast<T>( 1 ) );
}

/// Implementation inspired from https://en.cppreference.com/w/cpp/types/numeric_limits/epsilon
template <class T>
typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
areApproxEqual( T x, T y, T espilonBoostFactor ) {
    return std::abs( x - y ) <= std::numeric_limits<T>::epsilon() * espilonBoostFactor
           // unless the result is subnormal
           || std::abs( x - y ) < std::numeric_limits<T>::min();
}

template <typename T>
constexpr T lerp( const T& a, const T& b, Scalar t ) {
    return ( 1 - t ) * a + t * b;
}
} // namespace Math
} // namespace Core
} // namespace Ra
