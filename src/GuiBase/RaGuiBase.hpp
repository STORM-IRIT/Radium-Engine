#pragma once
#include <Core/CoreMacros.hpp>
#include <Engine/RaEngine.hpp>

/// Defines the correct macro to export dll symbols.
#if defined RA_GUIBASE_EXPORTS
#    define RA_GUIBASE_API DLL_EXPORT
#elif defined RA_GUIBASE_STATIC
#    define RA_GUIBASE_API
#else
#    define RA_GUIBASE_API DLL_IMPORT
#endif
