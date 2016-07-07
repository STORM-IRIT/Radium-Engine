#include <Core/CoreMacros.hpp>
#ifndef RADIUMENGINE_RA_ENGINE_HPP_
#define RADIUMENGINE_RA_ENGINE_HPP_

/// Defines the correct macro to export dll symbols.
#if defined   RA_ENGINE_EXPORTS    
    #define RA_ENGINE_API DLL_EXPORT 
#elif defined RA_ENGINE_STATIC
    #define RA_ENGINE_API 
#else
    #define RA_ENGINE_API DLL_IMPORT
#endif

#endif //RADIUMENGINE_RA_ENGINE_HPP_
