#ifndef RADIUMENGINE_ASSIMP_HANDLE_DATA_LOADER_HPP
#define RADIUMENGINE_ASSIMP_HANDLE_DATA_LOADER_HPP

#include <map>
#include <set>

#include <Core/Asset/DataLoader.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <IO/RaIO.hpp>

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiBone;

namespace Ra {
namespace Core {
namespace Asset {
class HandleData;
struct HandleComponentData;
} // namespace Asset
} // namespace Core
} // namespace Ra

namespace Ra {
namespace IO {

class RA_IO_API AssimpHandleDataLoader : public Core::Asset::DataLoader<Core::Asset::HandleData> {
  public:
    /// CONSTRUCTOR
    AssimpHandleDataLoader( const bool VERBOSE_MODE = false );

    /// DESTRUCTOR
    ~AssimpHandleDataLoader();

    /// LOAD
    void loadData( const aiScene* scene,
                   std::vector<std::unique_ptr<Core::Asset::HandleData>>& data ) override;

  protected:
    /// QUERY
    bool sceneHasHandle( const aiScene* scene ) const;
    uint sceneHandleSize( const aiScene* scene ) const;

    /// LOAD
    void loadHandleData( const aiScene* scene,
                         std::vector<std::unique_ptr<Core::Asset::HandleData>>& data ) const;
    void loadHandleComponentData( const aiScene* scene, const aiMesh* mesh,
                                  Core::Asset::HandleData* data ) const;
    void loadHandleComponentData( const aiScene* scene, const aiBone* bone,
                                  Core::Asset::HandleComponentData& data ) const;
    void loadHandleComponentData( const aiNode* node, Core::Asset::HandleComponentData& data ) const;
    void loadHandleTopologyData( const aiScene* scene, Core::Asset::HandleData* data ) const;
    void loadHandleFrame( const aiNode* node, const Core::Math::Transform& parentFrame,
                          const std::map<uint, uint>& indexTable,
                          std::vector<std::unique_ptr<Core::Asset::HandleData>>& data ) const;

    /// NAME
    void fetchName( const aiMesh& mesh, Core::Asset::HandleData& data,
                    std::set<std::string>& usedNames ) const;

    /// TYPE
    void fetchType( const aiMesh& mesh, Core::Asset::HandleData& data ) const;

    /// VERTEX SIZE
    void fetchVertexSize( Core::Asset::HandleData& data ) const;
};

} // namespace IO
} // namespace Ra

#endif // RADIUMENGINE_ASSIMP_HANDLE_DATA_LOADER_HPP
