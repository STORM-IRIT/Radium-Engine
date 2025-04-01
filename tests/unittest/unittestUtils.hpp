#pragma once

#include <Core/Math/Math.hpp>
#include <Core/Utils/StdOptional.hpp>
#include <Core/Utils/TypesUtils.hpp>

// need StdOptional included before
#ifdef have_optional
template <typename S>
inline bool isApprox( const Ra::Core::Utils::optional<S>& lhs, const S& rhs ) {
    return lhs.has_value() && Ra::Core::Math::areApproxEqual( *lhs, rhs );
}

template <typename S>
inline bool isApprox( const S& lhs, const Ra::Core::Utils::optional<S>& rhs ) {
    return rhs.has_value() && Ra::Core::Math::areApproxEqual( lhs, *rhs );
}
#endif

template <typename S>
inline bool isApprox( const S& lhs, const S& rhs ) {
    return Ra::Core::Math::areApproxEqual( lhs, rhs );
}

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
    NonCopy( const NonCopy& )            = delete;
    NonCopy& operator=( const NonCopy& ) = delete;
};

/// https://gist.github.com/szymek156/9b1b90fe474277be4641e9ef4666f472
template <class Class>
struct has_ostream_operator_impl {
    template <class V>
    static auto test( V* ) -> decltype( std::declval<std::ostream>() << std::declval<V>() );
    template <typename>
    static auto test( ... ) -> std::false_type;

    using type = typename std::is_same<std::ostream&, decltype( test<Class>( 0 ) )>::type;
};

template <class Class>
struct has_ostream_operator : has_ostream_operator_impl<Class>::type {};

/// Types, list of types as Ra::Core::Utils::TypeList< ... >
template <typename Types>
struct PrintAllHelper {
    using types = Types;

    template <typename T,
              typename std::enable_if<has_ostream_operator<T>::value, bool>::type = true>
    void operator()( const std::string& name, T& value ) {
        std::cout << " [ " << name << " --> " << value << " ("
                  << Ra::Core::Utils::simplifiedDemangledType<T>() << ") ]\n";
    }

    template <typename T,
              typename std::enable_if<!has_ostream_operator<T>::value, bool>::type = true>
    void operator()( const std::string& name, const T& ) {
        std::cout << " [ " << name << " --> ___ (" << Ra::Core::Utils::simplifiedDemangledType<T>()
                  << ") ]\n";
    }
};
