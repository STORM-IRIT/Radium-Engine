#ifndef RADIUMENGINE_VERSION_HPP
#define RADIUMENGINE_VERSION_HPP

#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {
namespace Utils {

RA_CORE_API extern const char* compiler;
RA_CORE_API extern const char* compileFlags;
RA_CORE_API extern const char* compileDate;
RA_CORE_API extern const char* compileTime;
RA_CORE_API extern const char* gitChangeSet;

// TODO :

// extern const char* gitTag;
// extern const char* gitVersion;
// extern const char* versionName;
} // namespace Utils
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_VERSION_HPP
