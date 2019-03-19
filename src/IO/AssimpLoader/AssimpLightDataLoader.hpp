#ifndef RADIUMENGINE_ASSIMP_LIGHT_DATA_LOADER_HPP
#define RADIUMENGINE_ASSIMP_LIGHT_DATA_LOADER_HPP

#include <Core/Asset/DataLoader.hpp>
#include <Core/Asset/LightData.hpp>
#include <Core/Math/Types.hpp>
#include <IO/RaIO.hpp>

#include <set>

struct aiScene;
struct aiLight;
struct aiNode;

namespace Ra {
namespace Engine {
class Light;
} // namespace Engine
} // namespace Ra

namespace Ra {
namespace IO {
/**
 * The AssimpHandleDataLoader converts light data from the Assimp format
 * to the Asset::LightData format.
 */
class RA_IO_API AssimpLightDataLoader : public Core::Asset::DataLoader<Core::Asset::LightData> {
  public:
    explicit AssimpLightDataLoader( const std::string& filepath, const bool VERBOSE_MODE = false );

    ~AssimpLightDataLoader() override;

    void loadData( const aiScene* scene,
                   std::vector<std::unique_ptr<Core::Asset::LightData>>& data ) override;

  protected:
    /**
     * Return true if the given scene has light data.
     */
    inline bool sceneHasLight( const aiScene* scene ) const;

    /**
     * Return the number of lights in the given scene.
     */
    uint sceneLightSize( const aiScene* scene ) const;

    /**
     * Fill the returned LightData from \p light.
     */
    std::unique_ptr<Core::Asset::LightData> loadLightData( const aiScene* scene,
                                                           const aiLight& light );

    /**
     * Register the light frame.
     */
    inline void setFrame( const Core::Matrix4& frame ) { m_frame = frame; }

    /**
     * Fill \p data with the light name from \p light.
     */
    std::string fetchName( const aiLight& light ) const;

    /**
     * Fill \p data with the light type from \p light.
     */
    Core::Asset::LightData::LightType fetchType( const aiLight& light ) const;

    /**
     * Return the light transformation, in world space, for \p data from \p the scene.
     */
    Core::Matrix4 loadLightFrame( const aiScene* scene, const Core::Matrix4& parentFrame,
                                  const std::string& lightName ) const;

  private:
    /// The file.
    std::string m_filepath;

    /// The light frame.
    Core::Transform m_frame;
};

} // namespace IO
} // namespace Ra

#endif // RADIUMENGINE_ASSIMP_LIGHT_DATA_LOADER_HPP
