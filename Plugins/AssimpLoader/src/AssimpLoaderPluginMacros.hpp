#ifndef ASSIMPLOADERPLUGINMACROS_HPP_
#define ASSIMPLOADERPLUGINMACROS_HPP_

#include <Core/CoreMacros.hpp>

/// Defines the correct macro to export dll symbols.
#if defined AssimpLoader_EXPORTS
    #define AssimpLoader_PLUGIN_API DLL_EXPORT
#else
    #define AssimpLoader_PLUGIN_API DLL_IMPORT
#endif

#endif // ASSIMPLOADERPLUGINMACROS_HPP_
