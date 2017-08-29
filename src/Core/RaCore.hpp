#ifndef RADIUMENGINE_RA_CORE_HPP_
#define RADIUMENGINE_RA_CORE_HPP_

#include <Core/CoreMacros.hpp>

/// Defines the correct macro to export dll symbols.
#if defined   RA_CORE_EXPORTS    
    #define RA_CORE_API DLL_EXPORT 
#elif defined RA_CORE_STATIC
    #define RA_CORE_API 
#else
    #define RA_CORE_API DLL_IMPORT 
#endif

#endif //RADIUMENGINE_RA_CORE_HPP_
