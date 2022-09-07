#pragma once
#include <Core/CoreMacros.hpp>

/// Defines the correct macro to export dll symbols.
#if defined RA_DATAFLOW_EXPORTS
#    define RA_DATAFLOW_API DLL_EXPORT
#elif defined RA_DATAFLOW_STATIC
#    define RA_DATAFLOW_API
#else
#    define RA_DATAFLOW_API DLL_IMPORT
#endif
