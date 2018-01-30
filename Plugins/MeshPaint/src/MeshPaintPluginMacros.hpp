#ifndef MESHPAINTPLUGINMACROS_HPP_
#define MESHPAINTPLUGINMACROS_HPP_

#include <Core/CoreMacros.hpp>

/// Defines the correct macro to export dll symbols.
#if defined  MeshPaint_EXPORTS
    #define MESH_PAINT_PLUGIN_API DLL_EXPORT
#else
    #define MESH_PAINT_PLUGIN_API DLL_IMPORT
#endif

#endif // MESHPAINTPLUGINMACROS_HPP_
