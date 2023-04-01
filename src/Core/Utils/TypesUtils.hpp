#pragma once

#include <Core/CoreMacros.hpp>

#ifndef _WIN32
#    include <cxxabi.h>
#    include <memory>
#else
#    include <typeinfo>
#endif

#include <string>

#include <Core/Utils/StringUtils.hpp>

namespace Ra {
namespace Core {
namespace Utils {

/// Return the human readable version of the type name T
template <typename T>
const char* demangleType() noexcept;

/// Return the human readable version of the given object's type
template <typename T>
const char* demangleType( const T& ) noexcept;

// TypeList taken and adapted from
// https://github.com/AcademySoftwareFoundation/openvdb/blob/master/openvdb/openvdb/TypeList.h
// Only took small part of TypeList utilities

// forward declarations
template <typename... Ts>
struct TypeList;

namespace TypeListInternal {

/// \brief   Append any number of types to a @c TypeList
/// \details Defines a new @c TypeList with the provided types appended
/// \tparam ListT  The @c TypeList to append to
/// \tparam Ts     Types to append
template <typename ListT, typename... Ts>
struct TSAppendImpl;

/// \brief  Partial specialization for a @c TypeList with a list of zero or more
///         types to append
/// \tparam Ts Current types within the @c TypeList
/// \tparam OtherTs Other types to append
template <typename... Ts, typename... OtherTs>
struct TSAppendImpl<TypeList<Ts...>, OtherTs...> {
    using type = TypeList<Ts..., OtherTs...>;
};

/// \brief  Partial specialization for a @c TypeList with another @c TypeList.
///         Appends the other TypeList's members.
/// \tparam Ts Types within the first @c TypeList
/// \tparam OtherTs Types within the second @c TypeList
template <typename... Ts, typename... OtherTs>
struct TSAppendImpl<TypeList<Ts...>, TypeList<OtherTs...>> {
    using type = TypeList<Ts..., OtherTs...>;
};

} // namespace TypeListInternal

template <typename... Ts>
struct TypeList {
    /// \brief The type of this list
    using Self = TypeList;
    /// \brief The number of types in the type list
    static constexpr size_t Size = sizeof...( Ts );

    /// \brief Append types, or the members of another TypeList, to this list.
    /// \warning Appending nested TypeList<> objects causes them to expand to
    ///          their contained list of types.
    /// \details Example:
    /// \code
    /// {
    ///     using IntTypes = Ra::Core::Utils::TypeList<Int16, Int32, Int64>;
    ///     using RealTypes = Ra::Core::Utils::TypeList<float, double>;
    ///     using NumericTypes = IntTypes::Append<RealTypes>;
    /// }
    /// {
    ///     using IntTypes = Ra::Core::Utils::TypeList<Int16>::Append<Int32, Int64>;
    ///     using NumericTypes = IntTypes::Append<float>::Append<double>;
    /// }
    /// \endcode
    template <typename... TypesToAppend>
    using Append = typename TypeListInternal::TSAppendImpl<Self, TypesToAppend...>::type;
};

#ifdef _WIN32
// On windows (since MSVC 2019), typeid( T ).name() returns the demangled name
template <typename T>
const char* demangleType() noexcept {
    static auto demangled_name = []() {
        std::string retval { typeid( T ).name() };
        removeAllInString( retval, "class " );
        removeAllInString( retval, "struct " );
        replaceAllInString( retval, ",", ", " );
        return retval;
    }();

    return demangled_name.data();
}
#else
template <typename T>
const char* demangleType() noexcept {
    // once per one type
    static auto demangled_name = []() {
        int error = 0;
        std::string retval;
        char* name = abi::__cxa_demangle( typeid( T ).name(), 0, 0, &error );

        switch ( error ) {
        case 0:
            retval = name;
            break;
        case -1:
            retval = "memory allocation failed";
            break;
        case -2:
            retval = "not a valid mangled name";
            break;
        default:
            retval = "__cxa_demangle failed";
            break;
        }
        std::free( name );
        removeAllInString( retval, "__1::" ); // or "::__1" ?
        replaceAllInString( retval, "> >", ">>" );
        return retval;
    }();

    return demangled_name.data();
}
#endif
// calling with instances
template <typename T>
const char* demangleType( const T& ) noexcept {
    return demangleType<T>();
}

} // namespace Utils
} // namespace Core
} // namespace Ra
