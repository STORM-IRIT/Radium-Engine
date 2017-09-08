#ifndef MESHFEATURETRACKINGPLUGINMACROS_HPP_
#define MESHFEATURETRACKINGPLUGINMACROS_HPP_

#include <Core/CoreMacros.hpp>

/// Defines the correct macro to export dll symbols.
#if defined  MeshFeatureTracking_EXPORTS
    #define MESH_FEATURE_TRACKING_PLUGIN_API DLL_EXPORT
#else
    #define MESH_FEATURE_TRACKING_PLUGIN_API DLL_IMPORT
#endif

#endif // MESHFEATURETRACKINGPLUGINMACROS_HPP_
