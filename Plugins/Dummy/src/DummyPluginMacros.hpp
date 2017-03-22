#ifndef DUMMYPLUGIN_DUMMYPLUGINMACROS_HPP
#define DUMMYPLUGIN_DUMMYPLUGINMACROS_HPP

#include <Core/CoreMacros.hpp>

/// Defines the correct macro to export dll symbols.
#if defined  Dummy_EXPORTS
    #define DUMMY_PLUGIN_API DLL_EXPORT
#else
    #define DUMMY_PLUGIN_API DLL_IMPORT
#endif

#endif // FANCYMESHPLUGINMACROS_HPP_
