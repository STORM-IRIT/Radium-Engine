#ifndef RADIUMENGINE_CAMERAMANAGER_H
#define RADIUMENGINE_CAMERAMANAGER_H

#include <Engine/Managers/CameraManager/CameraStorage.hpp>
#include <Engine/RaEngine.hpp>
#include <Engine/Renderer/RenderTechnique/RenderParameters.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/Renderer.hpp>
#include <Engine/System/System.hpp>

#include <memory>

namespace Ra {
namespace Engine {
class RenderObject;
} // namespace Engine
} // namespace Ra

namespace Ra {
namespace Engine {

/**
 * Interface providing functions to manage a group or type of Cameras
 * in a specific way.
 */
class RA_ENGINE_API CameraManager : public System {
    // TODO (Mathias) make Camera manager compatible with range for ...
  public:
    /// Constructor
    CameraManager();

    // Make copies impossible
    CameraManager( const CameraManager& ) = delete;
    CameraManager& operator=( const CameraManager& ) = delete;

    /// Virtual destructor
    virtual ~CameraManager();

    /// Get a pointer to the cam-th Camera.
    virtual const Camera* getCamera( size_t cam ) const = 0;

    /// Add a Camera to the manager ...
    virtual void addCamera( Camera* cam ) = 0;

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

    /**
     * @brief Call before a render, update the general state of the CameraManager.
     */
    virtual void preprocess( const RenderData& renderData ) = 0;

    /**
     * @brief Call before a render, process what is needed for a given Camera.
     */
    virtual void prerender( unsigned int cam ) = 0;

    /**
     * @brief render the object with specific technics for the current Camera (between prerender and
     * postrender)
     */
    virtual void
    render( RenderObject*, unsigned int cam,
            RenderTechnique::PassName passname = RenderTechnique::LIGHTING_OPAQUE ) = 0;

    /**
     * @brief Call after a render, process what is needed for a given Camera.
     */
    virtual void postrender( unsigned int cam ) = 0;

    /**
     * @brief Call after a render, update the general state of the CameraManager.
     */
    virtual void postprocess() = 0;

    //
    // System methods
    //

    /// Inherited method marked as final to ensure correct memory management
    /// even in child classes (e.g. CameraStorage).
    void registerComponent( const Entity* entity, Component* component ) override final;

    /// Inherited method marked as final to ensure correct memory management
    /// even in child classes (e.g. CameraStorage).
    void unregisterComponent( const Entity* entity, Component* component ) override final;

    /// Inherited method marked as final to ensure correct memory management
    /// even in child classes (e.g. CameraStorage).
    void unregisterAllComponents( const Entity* entity ) override final;

    void generateTasks( Core::TaskQueue* taskQueue, const Engine::FrameInfo& frameInfo ) override;

    void handleAssetLoading( Entity* entity, const Asset::FileData* data ) override;

  protected:
    /// store the current renderData
    RenderData renderData;
    /// store the current Camera parameters
    RenderParameters params;
    /// Stores the object that stores the Cameras...
    std::unique_ptr<CameraStorage> m_data;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_CAMERAMANAGER_H
