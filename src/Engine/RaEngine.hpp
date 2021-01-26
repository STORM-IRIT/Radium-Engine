#pragma once
#include <Core/CoreMacros.hpp>

/// Defines the correct macro to export dll symbols.
#if defined RA_ENGINE_EXPORTS
#    define RA_ENGINE_API DLL_EXPORT
#elif defined RA_ENGINE_STATIC
#    define RA_ENGINE_API
#else
#    define RA_ENGINE_API DLL_IMPORT
#endif

// always include engine's gl before any other
#include <Engine/Renderer/OpenGL/OpenGL.hpp>
