#ifndef PBRTLOADERPLUGINMACROS_HPP_
#define PBRTLOADERPLUGINMACROS_HPP_

#include <Core/CoreMacros.hpp>

/// Defines the correct macro to export dll symbols.
#if defined PbrtLoader_EXPORTS
#define PbrtLoader_PLUGIN_API DLL_EXPORT
#else
#define PbrtLoader_PLUGIN_API DLL_IMPORT
#endif

#endif // PBRTLOADERPLUGINMACROS_HPP_
