#ifndef FANCYMESHPLUGIN_HPP_
#define FANCYMESHPLUGIN_HPP_ 

#include <Core/CoreMacros.hpp>
/// Defines the correct macro to export dll symbols.
#if defined  FancyMesh_EXPORTS    
    #define FM_PLUGIN_API DLL_EXPORT 
#elif defined FancyMesh_IMPORTS
    #define FM_PLUGIN_API DLL_IMPORT 
#else
    #define FM_PLUGIN_API 
#endif

#endif // FANCYMESHPLUGIN_HPP_  