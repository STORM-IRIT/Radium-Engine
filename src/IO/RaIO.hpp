#include <Core/CoreMacros.hpp>
#ifndef RADIUMENGINE_RA_IO_HPP_
#define RADIUMENGINE_RA_IO_HPP_

/// Defines the correct macro to export dll symbols.
#if defined RA_IO_EXPORTS
    #define RA_IO_API DLL_EXPORT
#elif defined RA_IO_STATIC
    #define RA_IO_API
#else
    #define RA_IO_API DLL_IMPORT
#endif
#endif //RADIUMENGINE_RA_IO_HPP_
