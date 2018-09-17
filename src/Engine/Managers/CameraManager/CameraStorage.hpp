#ifndef RADIUMENGINE_CAMERASTORAGE_HPP
#define RADIUMENGINE_CAMERASTORAGE_HPP

#include <Engine/RaEngine.hpp>
#include <Engine/Renderer/Camera/Camera.hpp>

namespace Ra {
namespace Engine {
class RenderParameters;
} // namespace Engine
} // namespace Ra

namespace Ra {
namespace Engine {

/**
 * Interface providing functions to access and store cameras in
 * an implementation-defined way.
 */
class RA_ENGINE_API CameraStorage {
    // TODO: make camera storage compatible with range for ...
  public:
    CameraStorage() {}

    /// Destructor
    virtual ~CameraStorage() {}

    // Redefine container classic functions.

    /// Returns the container size.
    virtual size_t size() const = 0;

    /// Add a Camera to the container.
    virtual void add( Camera* cam ) = 0;

    /// Remove a Camera from the container.
    virtual void remove( Camera* cam ) = 0;

    /// Clear the container.
    virtual void clear() = 0;

    /// Access the container.
    virtual Camera* operator[]( unsigned int n ) = 0;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_CAMERASTORAGE_HPP
