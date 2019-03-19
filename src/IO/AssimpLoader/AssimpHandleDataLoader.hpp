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

namespace Ra {
namespace Core {
namespace Asset {
class HandleData;
struct HandleComponentData;
} // namespace Asset
} // namespace Core

namespace IO {

/**
 * The AssimpHandleDataLoader converts animation skeleton data from the Assimp
 * format to the Asset::HandleData format.
 */
// FIXME: Does AssImp handle other types like cages or point sets?
class RA_IO_API AssimpHandleDataLoader : public Core::Asset::DataLoader<Core::Asset::HandleData> {
  public:
    explicit AssimpHandleDataLoader( const bool VERBOSE_MODE = false );

    ~AssimpHandleDataLoader() override;

    void loadData( const aiScene* scene,
                   std::vector<std::unique_ptr<Core::Asset::HandleData>>& data ) override;

  protected:
    /**
     * Return true if the given scene has animation skeleton data.
     */
    bool sceneHasHandle( const aiScene* scene ) const;

    /**
     * Return the number of animation skeletons in the given scene.
     */
    uint sceneHandleSize( const aiScene* scene ) const;

    /**
     * Fill \p data with all the HandleData from \p scene.
     */
    void loadHandleData( const aiScene* scene,
                         std::vector<std::unique_ptr<Core::Asset::HandleData>>& data ) const;

    /**
     * Fill \p data with the name from \p mesh.
     * \note If the name is already in use, then appends as much "_" as needed.
     */
    void fetchName( const aiMesh& mesh, Core::Asset::HandleData& data,
                    std::set<std::string>& usedNames ) const;

    /**
     * Fill \p data with the animation skeleton from \p mesh, thus Asset::HandleData::SKELETON.
     */
    void fetchType( const aiMesh& mesh, Core::Asset::HandleData& data ) const;

    /**
     * Convert the animation skeleton data from the given scene object into \p data.
     */
    void loadHandleComponentData( const aiScene* scene, const aiMesh* mesh,
                                  Core::Asset::HandleData* data ) const;

    /**
     * Convert the skinning weights for the given animation bone from the given scene into \p data.
     */
    void loadHandleComponentData( const aiScene* scene, const aiBone* bone,
                                  Core::Asset::HandleComponentData& data ) const;

    /**
     * Convert the animation bone transformation, relative to the skeleton transform,
     * from the given scene node into \p data.
     */
    void loadHandleComponentData( const aiNode* node,
                                  Core::Asset::HandleComponentData& data ) const;

    /**
     * Convert the animation skeleton hierarchy from the given scene into \p data.
     */
    void loadHandleTopologyData( const aiScene* scene, Core::Asset::HandleData* data ) const;

    /**
     * Compute the maximal number of vertices influenced by the given animation skeleton.
     */
    void fetchVertexSize( Core::Asset::HandleData& data ) const;

    /**
     * Convert the animation skeleton transformation, relative to the scene transformation,
     * from the given scene node into \p data.
     */
    void loadHandleFrame( const aiNode* node, const Core::Transform& parentFrame,
                          const std::map<uint, uint>& indexTable,
                          std::vector<std::unique_ptr<Core::Asset::HandleData>>& data ) const;
};

} // namespace IO
} // namespace Ra

#endif // RADIUMENGINE_ASSIMP_HANDLE_DATA_LOADER_HPP
