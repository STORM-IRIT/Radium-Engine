#ifndef RADIUMENGINE_LIGHTMANAGER_H
#define RADIUMENGINE_LIGHTMANAGER_H
#include <Engine/Managers/LightManager/LightStorage.hpp>
#include <Engine/RaEngine.hpp>
#include <Engine/System/System.hpp>

#include <memory>

namespace Ra {
namespace Engine {

/**
 * Interface providing functions to manage a group or type of Lights
 * in a specific way.
 */
class RA_ENGINE_API LightManager : public System {
    // Radium-V2 : make light manager compatible with range for ...
  public:
    LightManager() = default;

    /**
     * Copy operator is forbidden.
     */
    LightManager( const LightManager& ) = delete;

    /**
     * Assignment operator is forbidden.
     */
    LightManager& operator=( const LightManager& ) = delete;

    ~LightManager() override;

    /**
     * Do nothing as this system only manages Light storage.
     */
    void generateTasks( Core::TaskQueue* taskQueue, const Engine::FrameInfo& frameInfo ) override;

    void handleAssetLoading( Entity* entity, const Core::Asset::FileData* data ) override;

    /**
     * Get a pointer to the li-th Light.
     */
    virtual const Light* getLight( size_t li ) const = 0;

    /**
     * Add a light to the manager.
     * Considers \p li is already registered. The LightManager will
     * not take ownership of the added Light, it will just push it in
     * the storage.
     */
    virtual void addLight( const Light* li ) = 0;

    /**
     * \brief Return the number of Lights.
     * This is still a work in progress. The idea is to make it possible for a
     * LightManager to tell it has only one Light, for example if it wants to send
     * a lot of sources at once in a single RenderParameters, let's say a texture.
     */
    virtual size_t count() const;

  protected:
    /**
     * Registers a Light belonging to an Entity.
     */
    void registerComponent( const Entity* entity, Component* component ) override final;

    /**
     * Unregisters a Light belonging to an Entity.
     */
    void unregisterComponent( const Entity* entity, Component* component ) override final;

    /**
     * Removes all Lights belonging to an Entity.
     */
    void unregisterAllComponents( const Entity* entity ) override final;

  protected:
    /// Light storage.
    std::unique_ptr<LightStorage> m_data{nullptr};
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_LIGHTMANAGER_H
