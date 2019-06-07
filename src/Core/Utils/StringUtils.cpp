#include <Core/Utils/StringUtils.hpp>

#include <Core/CoreMacros.hpp>

/*
 * NOMINMAX is already defined in CoreMacros.hpp ...

#ifdef COMPILER_MSVC
#    define NOMINMAX // Avoid C2039 MSVC compiler error
//#undef vsnprintf
//#define vsnprintf(buffer, count, format, argptr) vsnprintf_s(buffer, count, count, format, argptr)
#endif
*/

#include <algorithm> // std::min std::max
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <memory>
#include <sstream>

namespace Ra {
namespace Core {
namespace Utils {
std::string getFileExt( const std::string& str ) {
    std::string res;
    ulong pos = str.find_last_of( '.' );
    res       = pos != std::string::npos ? str.substr( pos + 1 ) : "";
    return res;
}

std::string getDirName( const std::string& path ) {
    // We remove any trailing slashes.
    ulong pos = path.find_last_not_of( '/' );

    // Don't strip the last / from "/"
    if ( pos == std::string::npos ) { pos = path.find_first_of( "/" ); }

    std::string res;
    res = path.substr( 0, pos + 1 );

    // Now find the previous slash and cut the string.
    pos = res.find_last_of( '/' );

    // The directory is actually "/" because the last slash is in first position.
    // In that case we should return "/"
    if ( pos == 0 ) { res = "/"; }
    else if ( pos != std::string::npos )
    { res = res.substr( 0, pos ); }
    else
    { res = "."; }

    return res;
}

std::string getBaseName( const std::string& path, bool keepExtension ) {
    std::string res;
    // We remove any trailing slashes.
    ulong pos = path.find_last_not_of( '/' );
    // Don't strip the last / from "/"
    if ( pos == std::string::npos ) { pos = path.find_first_of( "/" ); }

    res = path.substr( 0, pos + 1 );

    // Now find the previous slash and cut the string.
    pos = res.find_last_of( '/' );

    if ( pos != std::string::npos ) { res = res.substr( pos + 1 ); }

    if ( !keepExtension )
    {
        pos = res.find_last_of( '.' );
        if ( pos != std::string::npos ) { res = res.substr( 0, pos ); }
    }

    return res;
}

} // namespace Utils
} // namespace Core
} // namespace Ra
