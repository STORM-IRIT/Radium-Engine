#pragma once

#include <Engine/RaEngine.hpp>
#include <Engine/Scene/CameraStorage.hpp>
#include <Engine/Scene/System.hpp>

#include <Engine/Scene/CameraComponent.hpp>
#include <memory>

namespace Ra {
namespace Engine {
namespace Data {
class CameraComponent;
}

namespace Scene {
/**
 * Interface providing functions to manage a group or type of Cameras
 * in a specific way.
 */
class RA_ENGINE_API CameraManager : public System
{
    // Radium-V2 : make Camera manager compatible with range for ...
  public:
    /// Constructor
    CameraManager();

    // Make copies impossible
    CameraManager( const CameraManager& ) = delete;
    CameraManager& operator=( const CameraManager& ) = delete;

    /// Virtual destructor
    ~CameraManager() override = default;

    /// Get a pointer to the cam-th Camera.
    virtual const CameraComponent* getCamera( size_t cam ) const = 0;

    /// Add a Camera to the manager ...
    virtual void addCamera( CameraComponent* cam ) = 0;

    //
    // Calls for the Renderer
    //

    /**
     * @brief Number of Cameras.
     * This is still a work in progress. The idea is to make it possible for a
     * CameraManager to tell it has only one Camera, for example if it wants to send
     * a lot of sources at once in a single RenderParams, let's say a texture.
     */
    virtual size_t count() const;

    //
    // System methods
    //
    void generateTasks( Core::TaskQueue* taskQueue, const Engine::FrameInfo& frameInfo ) override;

    void handleAssetLoading( Entity* entity, const Core::Asset::FileData* data ) override;

    static Ra::Core::Utils::Camera defaultCamera;

  protected:
    /** Inherited method marked as final to ensure correct memory management
     *  even in child classes (e.g. CameraStorage).
     */
    void registerComponent( const Entity* entity, Component* component ) final;

    /// Inherited method marked as final to ensure correct memory management
    /// even in child classes (e.g. CameraStorage).
    void unregisterComponent( const Entity* entity, Component* component ) final;

    /// Inherited method marked as final to ensure correct memory management
    /// even in child classes (e.g. CameraStorage).
    void unregisterAllComponents( const Entity* entity ) final;

  protected:
    /// Stores the object that stores the Cameras...
    std::unique_ptr<CameraStorage> m_data {nullptr};
};

} // namespace Scene
} // namespace Engine
} // namespace Ra
