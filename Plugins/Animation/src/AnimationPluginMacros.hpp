#ifndef ANIMATIONPLUGINMACROS_HPP_
#define ANIMATIONPLUGINMACROS_HPP_

#include <Core/CoreMacros.hpp>

/// Defines the correct macro to export dll symbols.
#if defined Animation_EXPORTS
#    define ANIM_PLUGIN_API DLL_EXPORT
#else
#    define ANIM_PLUGIN_API DLL_IMPORT
#endif

#endif // ANIMATIONPLUGINMACROS_HPP_
