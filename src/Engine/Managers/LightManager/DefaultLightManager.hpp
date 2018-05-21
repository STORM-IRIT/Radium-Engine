#ifndef RADIUMENGINE_DUMMYLIGHTMANAGER_HPP
#define RADIUMENGINE_DUMMYLIGHTMANAGER_HPP

#include <Engine/Managers/LightManager/LightManager.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Light/DirLight.hpp>

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
    void add(Light *i) override;
    void remove(Light* li) override;
    void upload() const override;
    size_t size() const override;
    void clear() override;
    Light* operator[]( unsigned int n ) override;

  private:
    /** Vectors (by light type) of light references. */
    std::multimap<Ra::Engine::Light::LightType, Ra::Engine::Light*> m_lights;
};

/**
 * @brief DummyLightManager. A simple Light Manager with a list of lights.
 */
class RA_ENGINE_API DefaultLightManager : public LightManager {
  public:
    DefaultLightManager();

    const Light* getLight( size_t li ) const override;
    void addLight( Light* li ) override;

    // Since this manager is dummy, it won't do anything here.
    void preprocess( const RenderData& ) override;
    void prerender( unsigned int li ) override;
    void render( RenderObject*, unsigned int li,
                 RenderTechnique::PassName passname = RenderTechnique::LIGHTING_OPAQUE );
    void postrender( unsigned int li ) override;
    void postprocess() override;
};

} // namespace Engine
} // namespace Ra

#endif // DUMMYLIGHTMANAGER_HPP
