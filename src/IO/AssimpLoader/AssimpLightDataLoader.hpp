#ifndef RADIUMENGINE_ASSIMP_LIGHT_DATA_LOADER_HPP
#define RADIUMENGINE_ASSIMP_LIGHT_DATA_LOADER_HPP

#include <Core/Asset/DataLoader.hpp>
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

namespace Core{
namespace Asset {

class LightData;
}
}
} // namespace Ra

namespace Ra {
namespace IO {

class RA_IO_API AssimpLightDataLoader : public Core::Asset::DataLoader<Core::Asset::LightData> {
  public:
    /// CONSTRUCTOR
    AssimpLightDataLoader( const std::string& filepath, const bool VERBOSE_MODE = false );

    /// DESTRUCTOR
    ~AssimpLightDataLoader();

    /// LOADING
    void loadData( const aiScene* scene,
                   std::vector<std::unique_ptr<Core::Asset::LightData>>& data ) override;

  protected:
    /// QUERY
    inline bool sceneHasLight( const aiScene* scene ) const;

    uint sceneLightSize( const aiScene* scene ) const;

    /// LOADING

    void loadLightData( const aiScene* scene, const aiLight& light, Core::Asset::LightData& data );

    Core::Math::Matrix4 loadLightFrame( const aiScene* scene, const Core::Math::Matrix4& parentFrame,
                                  Core::Asset::LightData& data ) const;

    /// NAME
    void fetchName( const aiLight& mesh, Core::Asset::LightData& data ) const;

    /// TYPE
    void fetchType( const aiLight& mesh, Core::Asset::LightData& data ) const;

    /// FRAME
    inline void setFrame( const Core::Math::Matrix4& frame ) { m_frame = frame; }

  private:
    std::string m_filepath;
    Core::Math::Transform m_frame;
};

} // namespace IO
} // namespace Ra

#endif // RADIUMENGINE_ASSIMP_LIGHT_DATA_LOADER_HPP
