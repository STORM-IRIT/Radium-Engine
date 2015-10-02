#ifndef ANIMATIONPLUGIN_HPP_
#define ANIMATIONPLUGIN_HPP_

#include <Core/CoreMacros.hpp>
/// Defines the correct macro to export dll symbols.
#if defined  AnimationPlugin_EXPORTS
    #define ANIM_PLUGIN_API DLL_EXPORT
#elif defined AnimationPlugin_IMPORTS
    #define ANIM_PLUGIN_API DLL_IMPORT
#else
    #define ANIM_PLUGIN_API
#endif

#endif // ANIMATIONPLUGIN_HPP_