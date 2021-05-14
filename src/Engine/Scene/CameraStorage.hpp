#pragma once

#include <Engine/RaEngine.hpp>
#include <Engine/Scene/CameraComponent.hpp>

namespace Ra {
namespace Engine {

namespace Data {
class Camera;
}

namespace Scene {
class CameraComponent;

/**
 * Interface providing functions to access and store cameras in
 * an implementation-defined way.
 */
class RA_ENGINE_API CameraStorage
{
    // Radium-V2 : make camera storage compatible with range for ...
  public:
    CameraStorage() = default;

    /// Destructor
    virtual ~CameraStorage() = default;

    // Redefine container classic functions.

    /// Returns the container size.
    virtual size_t size() const = 0;

    /// Add a Camera to the container.
    virtual void add( CameraComponent* cam ) = 0;

    /// Remove a Camera from the container.
    virtual void remove( CameraComponent* cam ) = 0;

    /// Clear the container.
    virtual void clear() = 0;

    /// Access the container.
    virtual CameraComponent* operator[]( unsigned int n ) = 0;
};

} // namespace Scene
} // namespace Engine
} // namespace Ra
