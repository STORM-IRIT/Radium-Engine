#include <Core/Utils/StringUtils.hpp>
#include <Core/Utils/TypesUtils.hpp>

namespace Ra {
namespace Core {
namespace Utils {

#ifdef _WIN32
template <typename T>
const char* demangleType() noexcept {
    static auto demangled_name = typeid( T ).name();
    return demangled_name;
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
        return retval;
    }();
    removeAllInString( demangled_name, "__1::" ); // or "::__1" ?
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
