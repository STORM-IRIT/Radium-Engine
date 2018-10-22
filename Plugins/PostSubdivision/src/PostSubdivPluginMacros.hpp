#ifndef POSTSUBDIVPLUGINMACROS_HPP_
#define POSTSUBDIVPLUGINMACROS_HPP_

#include <Core/CoreMacros.hpp>

/// Defines the correct macro to export dll symbols.
#if defined POST_SUBDIV_EXPORTS
#    define POST_SUBDIV_PLUGIN_API DLL_EXPORT
#else
#    define POST_SUBDIV_PLUGIN_API DLL_IMPORT
#endif

#endif // POSTSUBDIVPLUGINMACROS_HPP_
