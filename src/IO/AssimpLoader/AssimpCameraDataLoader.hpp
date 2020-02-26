#ifndef RADIUMENGINE_ASSIMP_CAMERA_DATA_LOADER_HPP
#define RADIUMENGINE_ASSIMP_CAMERA_DATA_LOADER_HPP

#include <Core/Asset/DataLoader.hpp>
#include <Core/Types.hpp>
#include <IO/RaIO.hpp>

#include <set>

struct aiScene;
struct aiCamera;
struct aiNode;

namespace Ra {
namespace Engine {
class Camera;
} // namespace Engine

namespace Core {
namespace Asset {
class CameraData;
}
} // namespace Core

namespace IO {

/// The AssimpCameraDataLoader converts camera data from the Assimp format
/// to the Asset::CameraData format.
class RA_IO_API AssimpCameraDataLoader : public Core::Asset::DataLoader<Core::Asset::CameraData>
{
  public:
    explicit AssimpCameraDataLoader( const bool VERBOSE_MODE = false );

    ~AssimpCameraDataLoader() override;

    /// Convert all the camera data from \p scene into \p data.
    void loadData( const aiScene* scene,
                   std::vector<std::unique_ptr<Core::Asset::CameraData>>& data ) override;

  protected:
    /// Return true if the given scene has camera data.
    inline bool sceneHasCamera( const aiScene* scene ) const;

    /// Return the number of cameras in the given scene.
    uint sceneCameraSize( const aiScene* scene ) const;

    /// Fill \p data with the CameraData from \p camera.
    void
    loadCameraData( const aiScene* scene, const aiCamera& camera, Core::Asset::CameraData& data );

    /// Fill \p data with the camera name from \p camera.
    void fetchName( const aiCamera& camera, Core::Asset::CameraData& data ) const;

    /// Return the Camera transformation, in world space, for \p data from \p the scene.
    Core::Matrix4 loadCameraFrame( const aiScene* scene,
                                   const Core::Matrix4& parentFrame,
                                   Core::Asset::CameraData& data ) const;
};

} // namespace IO
} // namespace Ra

#endif // RADIUMENGINE_ASSIMP_CAMERA_DATA_LOADER_HPP
