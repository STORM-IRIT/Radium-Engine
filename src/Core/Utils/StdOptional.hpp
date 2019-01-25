#pragma once

/*
 * This file provides
 *  - Ra::Core::Utils::optional, which is an alias for std::optional or std::experimental::optional,
 *  - preprocessor symbols:
 *    - have_optional: defined if optional is found in std:: or in std::experimental::
 *    - have_experimental_optional: defined iff optional is found in std::experimental::
 *
 * To trigger an error if optional is not found, define RA_REQUIRE_OPTIONAL before including this
 * file:
 * \code{.cpp}
 * #define RA_REQUIRE_OPTIONAL
 * #include <Core/Utils/StdOptional.hpp> // trigger an error if optional is not found
 * #undef RA_REQUIRE_OPTIONAL
 * \endcode
 */

#ifdef __has_include
#    if __has_include( <optional>)
#        include <optional>
#        define have_optional
#    elif __has_include( <experimental/optional>)
#        include <experimental/optional>
#        define have_optional
#        define have_experimental_optional
#    else
#        ifdef RA_REQUIRE_OPTIONAL
#            error Feature Optionnal is required
#        endif
#    endif
#endif

// Create alias
#ifdef have_optional
namespace Ra {
namespace Core {
namespace Utils {
#    ifdef have_experimental_optional
template <typename T>
using optional = std::experimental::optional<T>;
#    else
template <typename T>
using optional = std::optional<T>;
#    endif
} // namespace Utils
} // namespace Core
} // namespace Ra
#endif
// end Create alias
