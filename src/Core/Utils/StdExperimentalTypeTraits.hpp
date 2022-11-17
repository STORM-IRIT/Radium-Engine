#pragma once

/*
 * This file provides
 *  - Ra::Core::Utils::is_detected, which is an alias for std::experimental::is_detected
 *  (see https://en.cppreference.com/w/cpp/experimental/is_detected) or which
 *  follows the sample implementation from
 *  https://en.cppreference.com/w/cpp/experimental/is_detected#Possible_implementation,
 *
 */

#ifdef __has_include
// cppcheck-suppress preprocessorErrorDirective
#    if __has_include( <experimental/type_traits> )
#        include <experimental/type_traits>
#        define have_experimental_type_traits
#    endif
#endif

namespace Ra {
namespace Core {
namespace Utils {
#ifdef have_experimental_type_traits
// use std::experimental definition
template <template <class...> class Op, class... Args>
using is_detected = typename std::experimental::is_detected<Op, Args...>;

template <template <class...> class Op, class... Args>
using detected_t = typename std::experimental::detected_t<Op, Args...>;

template <class Default, template <class...> class Op, class... Args>
using detected_or = typename std::experimental::detected_or<Default, Op, Args...>;

#else
// use possible implementation given in
// https://en.cppreference.com/w/cpp/experimental/is_detected#Possible_implementation
namespace detail {
template <class Default, class AlwaysVoid, template <class...> class Op, class... Args>
struct detector {
    using value_t = std::false_type;
    using type    = Default;
};

template <class Default, template <class...> class Op, class... Args>
struct detector<Default, std::void_t<Op<Args...>>, Op, Args...> {
    using value_t = std::true_type;
    using type    = Op<Args...>;
};
} // namespace detail

struct nonesuch {
    ~nonesuch()                 = delete;
    nonesuch( nonesuch const& ) = delete;
    void operator=( nonesuch const& ) = delete;
};

template <template <class...> class Op, class... Args>
using is_detected = typename detail::detector<nonesuch, void, Op, Args...>::value_t;

template <template <class...> class Op, class... Args>
using detected_t = typename detail::detector<nonesuch, void, Op, Args...>::type;

template <class Default, template <class...> class Op, class... Args>
using detected_or = detail::detector<Default, void, Op, Args...>;
#endif
} // namespace Utils
} // namespace Core
} // namespace Ra
