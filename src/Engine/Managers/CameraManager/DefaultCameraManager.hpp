#ifndef RADIUMENGINE_DUMMYCAMERAMANAGER_HPP
#define RADIUMENGINE_DUMMYCAMERAMANAGER_HPP

#include <Engine/Managers/CameraManager/CameraManager.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Camera/Camera.hpp>

#include <memory>
#include <vector>

namespace Ra {
namespace Engine {

/**
 * Associated class.
 */
class RA_ENGINE_API DefaultCameraStorage : public CameraStorage {
  public:
    DefaultCameraStorage();
    void add( Camera* cam ) override;
    void remove( Camera* cam ) override;
    void upload() const override; ///< Does nothing here.
    size_t size() const override;
    void clear() override;
    Camera* operator[]( unsigned int n ) override;

  private:
    /** Vectors (by Camera type) of Camera references. */
    std::multimap<Ra::Engine::Camera::ProjType, Ra::Engine::Camera*> m_Cameras;
};

/**
 * @brief DummyCameraManager. A simple Camera Manager with a list of Cameras.
 */
class RA_ENGINE_API DefaultCameraManager : public CameraManager {
  public:
    DefaultCameraManager();

    /// Return the \p cam-th light.
    const Camera* getCamera( size_t cam ) const override;

    /// Add \p cam for management.
    void addCamera( Camera* cam ) override;

    /// Does nothing here, only register the new RenderData to use.
    void preprocess( const RenderData& ) override;

    /// Does nothing here.
    void prerender( unsigned int cam ) override;

    void render( RenderObject*, unsigned int cam,
                 RenderTechnique::PassName passname = RenderTechnique::LIGHTING_OPAQUE );

    /// Does nothing here.
    void postrender( unsigned int cam ) override;

    /// Does nothing here.
    void postprocess() override;
};

} // namespace Engine
} // namespace Ra

#endif // DUMMYCAMERAMANAGER_HPP
