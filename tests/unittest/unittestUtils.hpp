#pragma once

#include <Core/Math/Math.hpp>

// A non-copyable move semantics struct
struct NonCopy {
    explicit NonCopy( int x ) : value( x ) {}
    NonCopy( NonCopy&& other ) {
        value       = other.value;
        other.value = 0;
    }
    NonCopy& operator=( NonCopy&& other ) {
        value       = other.value;
        other.value = 0;
        return *this;
    }

    int value;

  private:
    NonCopy( const NonCopy& ) = delete;
    NonCopy& operator=( const NonCopy& ) = delete;
};

// include <Core/Utils/StdOptional.hpp> before this file
#ifdef have_optional
template <typename S>
inline bool isApprox( const Core::Utils::optional<S>& lhs, const S& rhs ) {
    return lhs.has_value() && Ra::Core::Math::areApproxEqual( *lhs, rhs );
}

template <typename S>
inline bool isApprox( const S& lhs, const Core::Utils::optional<S>& rhs ) {
    return rhs.has_value() && Ra::Core::Math::areApproxEqual( lhs, *rhs );
}
#endif

template <typename S>
inline bool isApprox( const S& lhs, const S& rhs ) {
    return Ra::Core::Math::areApproxEqual( lhs, rhs );
}
