#pragma once

#include <Engine/RaEngine.hpp>
#include <Engine/Scene/Light.hpp>

namespace Ra {
namespace Engine {
namespace Data {
class RenderParameters;
}

namespace Scene {

/**
 * Interface providing functions to access and store lights in
 * an implementation-defined way.
 */
class RA_ENGINE_API LightStorage
{
    // Radium-V2 : make light storage compatible with range for ...
  public:
    /// Constructor
    LightStorage() = default;

    /// Destructor
    virtual ~LightStorage() = default;

    /**
     * Upload data to the GPU.
     */
    virtual void upload() const = 0;

    // Redefine container classic functions.

    /// Returns the container size.
    virtual size_t size() const = 0;

    /// Add a Light to the container.
    virtual void add( const Scene::Light* li ) = 0;

    /// Remove a Light from the container.
    virtual void remove( const Scene::Light* li ) = 0;

    /// Clear the container.
    virtual void clear() = 0;

    /// Access the container.
    virtual const Scene::Light* operator[]( unsigned int n ) = 0;
};

} // namespace Scene
} // namespace Engine
} // namespace Ra
