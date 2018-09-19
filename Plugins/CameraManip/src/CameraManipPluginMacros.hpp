#ifndef CAMERAMANIPPLUGINMACROS_HPP_
#define CAMERAMANIPPLUGINMACROS_HPP_

#include <Core/CoreMacros.hpp>

/// Defines the correct macro to export dll symbols.
#if defined CameraManip_EXPORTS
#    define CAMERA_MANIP_PLUGIN_API DLL_EXPORT
#else
#    define CAMERA_MANIP_PLUGIN_API DLL_IMPORT
#endif

#endif // CAMERAMANIPPLUGINMACROS_HPP_
