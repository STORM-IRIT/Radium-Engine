#ifndef RADIUMENGINE_DUMMYLIGHTMANAGER_HPP
#define RADIUMENGINE_DUMMYLIGHTMANAGER_HPP

#include <Engine/Managers/LightManager/LightManager.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Light/Light.hpp>

#include <memory>
#include <vector>

namespace Ra {
namespace Engine {

/**
 * Light storage for the DefaultLightManager.
 */
class RA_ENGINE_API DefaultLightStorage : public LightStorage {
  public:
    DefaultLightStorage();

    ~DefaultLightStorage() override = default;

    void add( const Light* i ) override;

    void remove( const Light* li ) override;

    void upload() const override;

    size_t size() const override;

    void clear() override;

    const Light* operator[]( unsigned int n ) override;

  private:
    /// Multimap (by light type) of Light references.
    std::multimap<Ra::Engine::Light::LightType, const Ra::Engine::Light*> m_lights;
};

/**
 * \brief DefaultLightManager. A simple LightManager with a list of Lights.
 */
class RA_ENGINE_API DefaultLightManager : public LightManager {
  public:
    DefaultLightManager();

    ~DefaultLightManager() override = default;

    /**
     * Return the \p li-th Ligth.
     */
    const Light* getLight( size_t li ) const override;

    /**
     * Add li for management.
     */
    void addLight( const Light* li ) override;
};

} // namespace Engine
} // namespace Ra

#endif // DUMMYLIGHTMANAGER_HPP
