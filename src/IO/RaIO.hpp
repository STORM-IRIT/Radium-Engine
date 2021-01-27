#pragma once
#include <Core/CoreMacros.hpp>

/// Defines the correct macro to export dll symbols.
#if defined RA_IO_EXPORTS
#    define RA_IO_API DLL_EXPORT
#elif defined RA_IO_STATIC
#    define RA_IO_API
#else
#    define RA_IO_API DLL_IMPORT
#endif
