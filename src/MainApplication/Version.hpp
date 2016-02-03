#ifndef RADIUMENGINE_VERSION_HPP
#define RADIUMENGINE_VERSION_HPP

#include <Core/CoreMacros.hpp>

namespace Ra {
    namespace Version {

        static const char* compileDate = __DATE__;
        static const char* compileTime = __TIME__;

        extern const char* compileFlags;
        extern const char* compiler;

        // TODO :

        //extern const char* gitTag;
        //extern const char* gitVersion;
        //extern const char* versionName;
    }
}

#endif // RADIUMENGINE_VERSION_HPP
