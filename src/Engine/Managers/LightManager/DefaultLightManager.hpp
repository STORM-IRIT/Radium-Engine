#ifndef RADIUMENGINE_DUMMYLIGHTMANAGER_HPP
#define RADIUMENGINE_DUMMYLIGHTMANAGER_HPP

#include <Engine/RadiumEngine.hpp>
#include <Engine/Managers/LightManager/LightManager.hpp>
#include <Engine/Renderer/Light/Light.hpp>

#include <memory>
#include <vector>

namespace Ra {
namespace Engine {

/**
 * Associated class.
 */
class RA_ENGINE_API DefaultLightStorage : public LightStorage {
  public:
    DefaultLightStorage();
    void add(const Light *i) override;
    void remove(const Light *li) override;
    void upload() const override;
    size_t size() const override;
    void clear() override;
    const Light* operator[]( unsigned int n ) override;

  private:
    /** Multimap (by light type) of light references. */
    std::multimap<Ra::Engine::Light::LightType, const Ra::Engine::Light*> m_lights;
};

/**
 * @brief DefaultLightManager. A simple Light Manager with a list of lights.
 */
class RA_ENGINE_API DefaultLightManager : public LightManager {
  public:
    DefaultLightManager();

    const Light* getLight( size_t li ) const override;
    void addLight(const Light *li) override;
};

} // namespace Engine
} // namespace Ra

#endif // DUMMYLIGHTMANAGER_HPP
