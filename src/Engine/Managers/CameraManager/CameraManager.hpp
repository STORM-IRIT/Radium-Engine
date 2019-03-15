#ifndef RADIUMENGINE_CAMERAMANAGER_H
#define RADIUMENGINE_CAMERAMANAGER_H

#include <Engine/Managers/CameraManager/CameraStorage.hpp>
#include <Engine/RaEngine.hpp>
#include <Engine/System/System.hpp>

#include <memory>

namespace Ra {
namespace Engine {
class Camera;
} // namespace Engine
} // namespace Ra

namespace Ra {
namespace Engine {

/**
 * Interface providing functions to manage a group or type of Cameras
 * in a specific way.
 */
class RA_ENGINE_API CameraManager : public System {
    // Radium-V2 : make Camera manager compatible with range for ...
  public:
    CameraManager() = default;

    /**
     * Copy operator is forbidden.
     */
    CameraManager( const CameraManager& ) = delete;

    /**
     * Assignement operator is forbidden.
     */
    CameraManager& operator=( const CameraManager& ) = delete;

    ~CameraManager() override = default;

    /**
     * Do nothing as this system only manages Camera storage.
     */
    void generateTasks( Core::TaskQueue* taskQueue, const Engine::FrameInfo& frameInfo ) override;

    void handleAssetLoading( Entity* entity, const Core::Asset::FileData* data ) override;

    /**
     * Get a pointer to the cam-th Camera.
     */
    virtual const Camera* getCamera( size_t cam ) const = 0;

    /**
     * Add a Camera to the manager.
     * Considers \p cam is already registered. The CameraManager will
     * not take ownership of the added Camera, it will just push it in
     * the storage.
     */
    virtual void addCamera( Camera* cam ) = 0;

    /**
     * \brief Return the number of Cameras.
     * This is still a work in progress. The idea is to make it possible for a
     * CameraManager to tell it has only one Camera, for example if it wants to send
     * a lot of sources at once in a single RenderParameters, let's say a texture.
     */
    virtual size_t count() const;

  protected:
    /**
     * Registers a Camera belonging to an Entity.
     */
    void registerComponent( const Entity* entity, Component* component ) override final;

    /**
     * Unregisters a Camera belonging to an Entity.
     */
    void unregisterComponent( const Entity* entity, Component* component ) override final;

    /**
     * Removes all Camera belonging to an Entity.
     */
    void unregisterAllComponents( const Entity* entity ) override final;

  protected:
    /// Camera storage.
    std::unique_ptr<CameraStorage> m_data{nullptr};
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_CAMERAMANAGER_H
