#pragma once

#include <Core/CoreMacros.hpp>

#ifndef _WIN32
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
const char* demangleType() noexcept;

/// Return the human readable version of the given object's type
template <typename T>
const char* demangleType( const T& ) noexcept;

} // namespace Utils
} // namespace Core
} // namespace Ra

#include <Core/Utils/TypesUtils.inl>
