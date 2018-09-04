#ifndef RADIUMENGINE_ASSIMP_LIGHT_DATA_LOADER_HPP
#define RADIUMENGINE_ASSIMP_LIGHT_DATA_LOADER_HPP

#include <Core/File/DataLoader.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <IO/RaIO.hpp>

#include <set>

struct aiScene;
struct aiLight;
struct aiNode;

namespace Ra {
namespace Engine {
class Light;
} // namespace Engine

namespace Asset {
class LightData;
} // namespace Asset
} // namespace Ra

namespace Ra {
namespace IO {

/// The AssimpHandleDataLoader converts light data from the Assimp format
/// to the Asset::LightData format.
class RA_IO_API AssimpLightDataLoader : public Asset::DataLoader<Asset::LightData> {
  public:
    AssimpLightDataLoader( const bool VERBOSE_MODE = false );

    ~AssimpLightDataLoader();

    /// Convert all the light data from \p scene into \p data.
    void loadData( const aiScene* scene,
                   std::vector<std::unique_ptr<Asset::LightData>>& data ) override;

  protected:
    /// Return true if the given scene has light data.
    inline bool sceneHasLight( const aiScene* scene ) const;

    /// Return the number of lights in the given scene.
    uint sceneLightSize( const aiScene* scene ) const;

    /// Fill \p data with the LightData from \p light.
    void loadLightData( const aiScene* scene, const aiLight& light, Asset::LightData& data );

    /// Fill \p data with the light name from \p light.
    void fetchName( const aiLight& light, Asset::LightData& data ) const;

    /// Fill \p data with the light type from \p light.
    void fetchType( const aiLight& light, Asset::LightData& data ) const;

    /// Return the light transformation, in world space, for \p data from \p the scene.
    Core::Matrix4 loadLightFrame( const aiScene* scene, const Core::Matrix4& parentFrame,
                                  const Asset::LightData& data ) const;
};

} // namespace IO
} // namespace Ra

#endif // RADIUMENGINE_ASSIMP_LIGHT_DATA_LOADER_HPP
