#ifndef RADIUMENGINE_ASSIMP_HANDLE_DATA_LOADER_HPP
#define RADIUMENGINE_ASSIMP_HANDLE_DATA_LOADER_HPP

#include <map>
#include <set>

#include <Core/Asset/DataLoader.hpp>
#include <Core/Types.hpp>
#include <IO/RaIO.hpp>

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiBone;
struct aiString;

namespace Ra {
namespace Core {
namespace Asset {
class HandleData;
struct HandleComponentData;
} // namespace Asset
} // namespace Core

namespace IO {

class RA_IO_API AssimpHandleDataLoader : public Core::Asset::DataLoader<Core::Asset::HandleData>
{
  public:
    /// CONSTRUCTOR
    explicit AssimpHandleDataLoader( const bool VERBOSE_MODE = false );

    /// DESTRUCTOR
    ~AssimpHandleDataLoader() override;

    /// LOAD
    void loadData( const aiScene* scene,
                   std::vector<std::unique_ptr<Core::Asset::HandleData>>& data ) override;

  private:
    /// LOAD
    void loadHandleData( const aiScene* scene,
                         std::vector<std::unique_ptr<Core::Asset::HandleData>>& data ) const;
    void loadHandleComponentDataFrame( const aiScene* scene,
                                       const aiString& boneName,
                                       Core::Asset::HandleComponentData& data ) const;
    void loadHandleComponentDataWeights( const aiBone* bone,
                                         const std::string& meshName,
                                         Core::Asset::HandleComponentData& data ) const;
    void
    fillHandleData( const std::string& node,
                    const std::vector<std::pair<std::string, std::string>>& edgeList,
                    const std::map<std::string, Core::Asset::HandleComponentData>& mapBone2Data,
                    std::map<std::string, uint>& nameTable,
                    Core::Asset::HandleData* data ) const;
};

} // namespace IO
} // namespace Ra

#endif // RADIUMENGINE_ASSIMP_HANDLE_DATA_LOADER_HPP
