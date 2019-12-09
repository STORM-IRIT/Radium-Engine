#include <Core/CoreMacros.hpp>
#ifndef RADIUMENGINE_RA_PLUGINBASE_HPP_
#    define RADIUMENGINE_RA_PLUGINBASE_HPP_

/// Defines the correct macro to export dll symbols.
#    if defined RA_PLUGINBASE_EXPORTS
#        define RA_PLUGINBASE_API DLL_EXPORT
#    elif defined RA_PLUGINBASE_STATIC
#        define RA_PLUGINBASE_API
#    else
#        define RA_PLUGINBASE_API DLL_IMPORT
#    endif
#endif // RADIUMENGINE_RA_PLUGINBASE_HPP_

/// The macro RADIUM_PLUGIN_METADATA defines the default metadata for a plugin.
/// \see cmake/pluginMetaDataDebug.json and pluginMetaDataRelease.json for the list of metadatas.
///
/// To add new metadata to your own plugin, you need to call directly Q_PLUGIN_METADATA with your
/// own file, which should includes all the metadata packaged with Radium
///
#ifdef PLUGIN_IS_COMPILED_WITH_DEBUG_INFO
#    define Q_RADIUM_PLUGIN_METADATA \
        Q_PLUGIN_METADATA( IID "RadiumEngine.PluginInterface" FILE "pluginMetaDataDebug.json" )
#else
#    define Q_RADIUM_PLUGIN_METADATA \
        Q_PLUGIN_METADATA( IID "RadiumEngine.PluginInterface" FILE "pluginMetaDataRelease.json" )
#endif
