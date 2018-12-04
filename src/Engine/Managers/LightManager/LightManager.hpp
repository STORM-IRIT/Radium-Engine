#ifndef RADIUMENGINE_LIGHTMANAGER_H
#define RADIUMENGINE_LIGHTMANAGER_H

#include <Engine/Managers/LightManager/LightStorage.hpp>
#include <Engine/RaEngine.hpp>
#include <Engine/Renderer/RenderTechnique/RenderParameters.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/System/System.hpp>

// This will be removed when ViewingParameter will not be necessary for LightManager
#include <Engine/Renderer/Camera/Camera.hpp>

#include <memory>

namespace Ra {
namespace Engine {
class RenderObject;
}
} // namespace Ra

namespace Ra {
namespace Engine {

/**
 * Interface providing functions to manage a group or type of lights
 * in a specific way.
 */
class RA_ENGINE_API LightManager : public System {
    // TODO (Mathias) make light manager compatible with range for ...
  public:
    /// Constructor
    LightManager() = default;

    // Make copies impossible
    LightManager( const LightManager& ) = delete;
    LightManager& operator=( const LightManager& ) = delete;

    /// Virtual destructor
    ~LightManager() override;

    /// Get a pointer to the li-th Light.
    virtual const Light* getLight( size_t li ) const = 0;

    /** Add a light to the manager ...
     * Consider the component is already registered. The light manager will not take ownership of the added light,
     * it will just push the light on the storage ...
     * @param li The (already registered) light to add.
     */
    virtual void addLight(const Light *li) = 0;

    //
    // Calls for the Renderer. Note that
    //

    /**
     * @brief Number of lights.
     * This is still a work in progress. The idea is to make it possible for a
     * LightManager to tell it has only one Light, for example if it wants to send
     * a lot of sources at once in a single RenderParams, let's say a texture.
     */
    virtual size_t count() const;

    /**
     * @brief Call before a render, update the general state of the LightManager.
     */
    virtual void preprocess( const ViewingParameters& renderData ) = 0;

    /**
     * @brief Call before a render, process what is needed for a given Light.
     */
    virtual void prerender( unsigned int li ) = 0;

    /**
     * @brief render the object with specific technics for the current light (between prerender and
     * postrender)
     */
    virtual void
    render( RenderObject*, unsigned int li,
            RenderTechnique::PassName passname = RenderTechnique::LIGHTING_OPAQUE ) = 0;

    /**
     * @brief Call after a render, process what is needed for a given Light.
     */
    virtual void postrender( unsigned int li ) = 0;

    /**
     * @brief Call after a render, update the general state of the LightManager.
     */
    virtual void postprocess() = 0;

    //
    // System methods
    //

    void generateTasks( Core::TaskQueue* taskQueue, const Engine::FrameInfo& frameInfo ) override;

    void handleAssetLoading( Entity* entity, const Asset::FileData* data ) override;

  protected:
    /// Inherited method marked as final to ensure correct memory management
    /// even in child classes (e.g. LightStorage).
    void registerComponent( const Entity* entity, Component* component ) final;

    /// Inherited method marked as final to ensure correct memory management
    /// even in child classes (e.g. LightStorage).
    void unregisterComponent( const Entity* entity, Component* component ) final;

    /// Inherited method marked as final to ensure correct memory management
    /// even in child classes (e.g. LightStorage).
    void unregisterAllComponents( const Entity* entity ) final;

  protected:
    /// store the current renderData
    ViewingParameters viewingParameters {};

    /// store the current light parameters
    RenderParameters renderParameters {};

    /// Stores the object that stores the lights...
    std::unique_ptr<LightStorage> m_data { nullptr };
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_LIGHTMANAGER_H
