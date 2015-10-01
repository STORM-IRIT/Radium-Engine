#include <Core/CoreMacros.hpp>
#ifndef FANCYMESHPLUGIN_HPP_ 
#define FANCYMESHPLUGIN_HPP_ 

/// Defines the correct macro to export dll symbols.
#if defined  FancyMeshPlugin_EXPORTS    
    #define FM_PLUGIN_API DLL_EXPORT 
#elif defined FancyMeshPlugin_IMPORTS
    #define FM_PLUGIN_API DLL_IMPORT 
#else
    #define FM_PLUGIN_API 
#endif

#endif // FANCYMESHPLUGIN_HPP_  