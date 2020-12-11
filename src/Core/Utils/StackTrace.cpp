#include <Core/Utils/StackTrace.hpp>
namespace Ra {
namespace Core {
namespace Utils {

#ifndef OS_WINDOWS

// A C++ function that will produce a stack trace with demangled function and method names.
#    include <cxxabi.h>   // for __cxa_demangle
#    include <dlfcn.h>    // for dladdr
#    include <execinfo.h> // for backtrace
#    include <sstream>
#    include <string>
#    if defined( COMPILER_CLANG )
#        pragma clang diagnostic ignored "-Wformat"
#        pragma clang diagnostic ignored "-Wformat-extra-args"
#    elif defined( COMPILER_GCC )
#        pragma GCC diagnostic ignored "-Wformat"
#        pragma GCC diagnostic ignored "-Wformat-extra-args"
#    endif

std::string StackTrace( int skip ) {
    void* callstack[128];
    const int nMaxFrames = sizeof( callstack ) / sizeof( callstack[0] );
    char buf[1024];
    int nFrames = backtrace( callstack, nMaxFrames );

    std::ostringstream trace_buf;
    for ( int i = skip; i < nFrames; i++ )
    {
        Dl_info info;
        if ( dladdr( callstack[i], &info ) )
        {
            char* demangled = nullptr;
            int status;
            demangled = abi::__cxa_demangle( info.dli_sname, NULL, 0, &status );
            snprintf( buf,
                      sizeof( buf ),
                      "%-3d %*0p %s + %td\n",
                      i,
                      int( 2 + sizeof( void* ) * 2 ),
                      callstack[i],
                      status == 0 ? demangled : info.dli_sname,
                      (char*)callstack[i] - (char*)info.dli_saddr );
            free( demangled );
        }
        else
        {
            snprintf( buf,
                      sizeof( buf ),
                      "%-3d %*0p\n",
                      i,
                      int( 2 + sizeof( void* ) * 2 ),
                      callstack[i] );
        }
        trace_buf << buf;
    }
    if ( nFrames == nMaxFrames ) trace_buf << "  [truncated]\n";
    return trace_buf.str();
}

#else

inline std::string Backtrace( int skip = 1 ) {
    return "No execution stack trace on windows.";
}

#endif

} // namespace Utils
} // namespace Core
} // namespace Ra
