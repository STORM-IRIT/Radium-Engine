#ifndef RADIUMENGINE_RESOURCES_HPP
#define RADIUMENGINE_RESOURCES_HPP
namespace Ra {
  namespace Core {

    /** Resources management for Radium.
     *  This namespace contains functions, classes and utilities for resource management.
     *  Resources are :
     *    - configuration file (ui, keymapping, ...)
     *    - default shaders
     *    - ...
     *
     *    @todo define and implement a portable interface for resource management.
     */
    namespace Resources
    {

      /// Get the base prefix to access the Radium resources in a filesystem.
      const char * getBaseDir();
    };
  }
}
#endif //RADIUMENGINE_RESOURCES_HPP
