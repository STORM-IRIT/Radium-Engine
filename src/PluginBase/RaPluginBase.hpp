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
