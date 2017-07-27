#ifndef RADIUMENGINE_POINTCLOUDPLUGINMACROS_HPP
#define RADIUMENGINE_POINTCLOUDPLUGINMACROS_HPP

#include <Core/CoreMacros.hpp>

/// Defines the correct macro to export dll symbols.
#if defined  PointCloud_EXPORTS
#define PCLOUD_PLUGIN_API DLL_EXPORT
#else
#define PCLOUD_PLUGIN_API DLL_IMPORT
#endif


#endif //RADIUMENGINE_POINTCLOUDPLUGINMACROS_HPP
