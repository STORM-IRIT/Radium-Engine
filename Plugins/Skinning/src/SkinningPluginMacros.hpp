#ifndef SKINNINGPLUGINMACROS_HPP_
#define SKINNINGPLUGINMACROS_HPP_

#include <Core/CoreMacros.hpp>

/// Defines the correct macro to export dll symbols.
#if defined  Skinning_EXPORTS
    #define SKIN_PLUGIN_API DLL_EXPORT
#else
    #define SKIN_PLUGIN_API DLL_IMPORT
#endif

#endif // SKINNINGPLUGINMACROS_HPP_
