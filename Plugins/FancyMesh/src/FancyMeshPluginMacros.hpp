#ifndef FANCYMESHPLUGINMACROS_HPP_
#define FANCYMESHPLUGINMACROS_HPP_

#include <Core/CoreMacros.hpp>

/// Defines the correct macro to export dll symbols.
#if defined FancyMesh_EXPORTS
#    define FM_PLUGIN_API DLL_EXPORT
#else
#    define FM_PLUGIN_API DLL_IMPORT
#endif

#endif // FANCYMESHPLUGINMACROS_HPP_
