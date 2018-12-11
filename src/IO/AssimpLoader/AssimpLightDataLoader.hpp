#ifndef RADIUMENGINE_ASSIMP_LIGHT_DATA_LOADER_HPP
#define RADIUMENGINE_ASSIMP_LIGHT_DATA_LOADER_HPP

#include <Core/File/DataLoader.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/File/LightData.hpp>
#include <IO/RaIO.hpp>

#include <set>

struct aiScene;
struct aiLight;
struct aiNode;

namespace Ra {
namespace Engine {
class Light;
}

} // namespace Ra

namespace Ra {
namespace IO {

class RA_IO_API AssimpLightDataLoader : public Asset::DataLoader<Asset::LightData> {
  public:
    /// CONSTRUCTOR
    explicit AssimpLightDataLoader( const std::string& filepath, const bool VERBOSE_MODE = false );

    /// DESTRUCTOR
    ~AssimpLightDataLoader() override;

    /// LOADING
    void loadData( const aiScene* scene,
                   std::vector<std::unique_ptr<Asset::LightData>>& data ) override;

  protected:
    /// QUERY
    inline bool sceneHasLight( const aiScene* scene ) const;

    uint sceneLightSize( const aiScene* scene ) const;

    /// LOADING

//    Asset::LightData *loadLightData(const aiScene *scene, const aiLight &light);
    std::unique_ptr<Asset::LightData> loadLightData(const aiScene *scene, const aiLight &light);

    Core::Matrix4 loadLightFrame( const aiScene* scene, const Core::Matrix4& parentFrame,
                                  const std::string & lightName ) const;

    /// NAME
    std::string fetchName(const aiLight &light) const;

    /// TYPE
    Asset::LightData::LightType fetchType( const aiLight& light ) const;

    /// FRAME
    inline void setFrame( const Core::Matrix4& frame ) { m_frame = frame; }

  private:
    std::string m_filepath;
    Core::Transform m_frame;
};

} // namespace IO
} // namespace Ra

#endif // RADIUMENGINE_ASSIMP_LIGHT_DATA_LOADER_HPP
