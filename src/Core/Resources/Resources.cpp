#include <Core/Resources/Resources.hpp>

#ifndef RADIUM_BIN_BASEDIR
#warning "RADIUM_BIN_BASEDIR not defined : application/lib may malfunction... "
#define RADIUM_BIN_BASEDIR ""
#endif

namespace Ra {
  namespace Core {
    namespace Resources {
      static const char * baseDir = RADIUM_BIN_BASEDIR;
      const char * getBaseDir() {
          return baseDir;
      }
    }
  }
}


