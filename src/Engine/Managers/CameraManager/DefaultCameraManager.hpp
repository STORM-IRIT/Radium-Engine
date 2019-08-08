#ifndef RADIUMENGINE_DUMMYCAMERAMANAGER_HPP
#define RADIUMENGINE_DUMMYCAMERAMANAGER_HPP

#include <Engine/Managers/CameraManager/CameraManager.hpp>
#include <Engine/Renderer/Camera/Camera.hpp>

#include <memory>
#include <vector>

namespace Ra::Engine {

/**
 * Associated class.
 */
class RA_ENGINE_API DefaultCameraStorage : public CameraStorage
{
  public:
    DefaultCameraStorage();
    void add( Camera* cam ) override;
    void remove( Camera* cam ) override;
    size_t size() const override;
    void clear() override;
    Camera* operator[]( unsigned int n ) override;

  private:
    /** Vectors (by Camera type) of Camera references. */
    std::multimap<Ra::Engine::Camera::ProjType, Ra::Engine::Camera*> m_Cameras;
};

/**
 * @brief DefaultCameraManager. A simple Camera Manager with a list of Cameras.
 */
class RA_ENGINE_API DefaultCameraManager : public CameraManager
{
  public:
    DefaultCameraManager();

    /// Return the \p cam-th camera.
    const Camera* getCamera( size_t cam ) const override;

    /// Add \p cam for management.
    void addCamera( Camera* cam ) override;
};

} // namespace Ra::Engine

#endif // DUMMYCAMERAMANAGER_HPP
