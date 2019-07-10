#include <Core/Resources/Resources.hpp>

#ifndef RADIUM_RESOURCES_BASEDIR
#    warning "RADIUM_RESOURCES_BASEDIR not defined : applications might not function outside of Radium Bundle "
#    define RADIUM_RESOURCES_BASEDIR ""
#endif

namespace Ra {
namespace Core {
namespace Resources {
static const char* baseDir = RADIUM_RESOURCES_BASEDIR;
const char* getBaseDir() {
    return baseDir;
}
} // namespace Resources
} // namespace Core
} // namespace Ra
