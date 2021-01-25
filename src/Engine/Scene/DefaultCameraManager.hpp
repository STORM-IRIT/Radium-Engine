#pragma once

#include <Engine/Scene/Camera.hpp>
#include <Engine/Scene/CameraManager.hpp>

#include <memory>
#include <vector>

namespace Ra {
namespace Engine {
namespace Scene {

/**
 * Associated class.
 */
class RA_ENGINE_API DefaultCameraStorage : public CameraStorage
{
  public:
    DefaultCameraStorage();
    void add( Data::Camera* cam ) override;
    void remove( Data::Camera* cam ) override;
    size_t size() const override;
    void clear() override;
    Data::Camera* operator[]( unsigned int n ) override;

  private:
    /** Vectors (by Camera type) of Camera references. */
    std::multimap<Data::Camera::ProjType, Data::Camera*> m_Cameras;
};

/**
 * @brief DefaultCameraManager. A simple Camera Manager with a list of Cameras.
 */
class RA_ENGINE_API DefaultCameraManager : public CameraManager
{
  public:
    DefaultCameraManager();

    /// Return the \p cam-th camera.
    const Data::Camera* getCamera( size_t cam ) const override;

    /// Add \p cam for management.
    void addCamera( Data::Camera* cam ) override;
};

} // namespace Scene
} // namespace Engine
} // namespace Ra
