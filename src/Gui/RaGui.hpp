#pragma once

#include <Core/CoreMacros.hpp>
#include <Engine/RaEngine.hpp>

/// Defines the correct macro to export dll symbols.
#if defined RA_GUI_EXPORTS
#    define RA_GUI_API DLL_EXPORT
#elif defined RA_GUI_STATIC
#    define RA_GUI_API
#else
#    define RA_GUI_API DLL_IMPORT
#endif
