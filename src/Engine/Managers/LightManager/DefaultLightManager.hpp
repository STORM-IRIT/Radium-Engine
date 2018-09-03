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
    void add( Light* li ) override;
    void remove( Light* li ) override;
    void upload() const override; ///< Does nothing here.
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

    /// Return the \p li-th light.
    const Light* getLight( size_t li ) const override;

    /// Add \p li for management.
    void addLight( Light* li ) override;

    /// Does nothing here, only register the new RenderData to use.
    void preprocess( const RenderData& ) override;

    /// Collect the Lights' RenderParameters.
    void prerender( unsigned int li ) override;

    void render( RenderObject*, unsigned int li,
                 RenderTechnique::PassName passname = RenderTechnique::LIGHTING_OPAQUE );

    /// Does nothing here.
    void postrender( unsigned int li ) override;

    /// Does nothing here.
    void postprocess() override;
};

} // namespace Engine
} // namespace Ra

#endif // DUMMYLIGHTMANAGER_HPP
