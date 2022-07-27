#pragma once

/**
 * Get the Human readable type name.
 * from https://dbj.org/cpp-human-readable-type-names-on-both-platforms/
 *
 */

#ifndef COMPILER_MSVC
#    include <cxxabi.h>
#    include <memory>
#else
#    include <typeinfo>
#endif

#include <string>

namespace Ra {
namespace Core {
namespace Utils {

/// Return the human readable version of the type name T
template <typename T>
const char* decypherType() noexcept;

/// Return the human readable version of the given object's type
template <typename T>
const char* decypherType( const T& ) noexcept;

} // namespace Utils
} // namespace Core
} // namespace Ra

#include <Core/Utils/TypesUtils.inl>
