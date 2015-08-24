#include <Core/CoreMacros.hpp>
#ifndef RIGIDBODYPLUGIN_HPP_ 
#define RIGIDBODYPLUGIN_HPP_ 

/// Defines the correct macro to export dll symbols.
#if defined   RigidBodyPlugin_EXPORTS    
    #define RB_PLUGIN_API DLL_EXPORT 
#elif defined RigidBodyPlugin_IMPORTS
    #define RB_PLUGIN_API DLL_IMPORT 
#else
    #define RB_PLUGIN_API 
#endif

#endif // RIGIDBODYPLUGIN_HPP_ 
