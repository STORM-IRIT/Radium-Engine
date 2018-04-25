#ifndef RADIUMENGINE_ASSIMP_GEOMETRY_DATA_LOADER_HPP
#define RADIUMENGINE_ASSIMP_GEOMETRY_DATA_LOADER_HPP

#include <set>

#include <Core/Asset/DataLoader.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <IO/RaIO.hpp>

struct aiScene;
struct aiMesh;
struct aiNode;
struct aiMaterial;

namespace Ra {
namespace Core {
namespace Asset {
class GeometryData;
} // namespace Asset
} // namespace Core
} // namespace Ra

namespace Ra {
namespace IO {

struct RA_IO_API Triplet {
    Triplet( const Core::Math::Vector3& v = Core::Math::Vector3::Zero() );

    Core::Math::Vector3 m_v;

    bool operator==( const Triplet& t ) const;

    bool operator<( const Triplet& t ) const;
};

class RA_IO_API AssimpGeometryDataLoader : public Core::Asset::DataLoader<Core::Asset::GeometryData> {
  public:
    /// CONSTRUCTORAssimpAnimationDataLoader
    AssimpGeometryDataLoader( const std::string& filepath, const bool VERBOSE_MODE = false );

    /// DESTRUCTOR
    ~AssimpGeometryDataLoader();

    /// LOADING
    void loadData( const aiScene* scene,
                   std::vector<std::unique_ptr<Core::Asset::GeometryData>>& data ) override;

  protected:
    /// QUERY
    inline bool sceneHasGeometry( const aiScene* scene ) const;

    uint sceneGeometrySize( const aiScene* scene ) const;

    /// LOADING
    void loadGeometryData( const aiScene* scene,
                           std::vector<std::unique_ptr<Core::Asset::GeometryData>>& data );

    void loadMeshData( const aiMesh& mesh, Core::Asset::GeometryData& data,
                       std::set<std::string>& usedNames );

    void loadMeshFrame( const aiNode* node, const Core::Math::Transform& parentFrame,
                        const std::map<uint, uint>& indexTable,
                        std::vector<std::unique_ptr<Core::Asset::GeometryData>>& data ) const;

    /// NAME
    void fetchName( const aiMesh& mesh, Core::Asset::GeometryData& data,
                    std::set<std::string>& usedNames ) const;

    /// TYPE
    void fetchType( const aiMesh& mesh, Core::Asset::GeometryData& data ) const;

    /// VERTEX
    void fetchVertices( const aiMesh& mesh, Core::Asset::GeometryData& data );

    /// EDGE
    void fetchEdges( const aiMesh& mesh, Core::Asset::GeometryData& data ) const;

    /// FACE
    void fetchFaces( const aiMesh& mesh, Core::Asset::GeometryData& data ) const;

    /// POLYHEDRON
    void fetchPolyhedron( const aiMesh& mesh, Core::Asset::GeometryData& data ) const;

    /// NORMAL
    void fetchNormals( const aiMesh& mesh, Core::Asset::GeometryData& data ) const;

    /// TANGENT
    void fetchTangents( const aiMesh& mesh, Core::Asset::GeometryData& data ) const;

    /// BITANGENT
    void fetchBitangents( const aiMesh& mesh, Core::Asset::GeometryData& data ) const;

    /// TEXTURE COORDINATE
    void fetchTextureCoordinates( const aiMesh& mesh, Core::Asset::GeometryData& data ) const;

    /// COLOR
    void fetchColors( const aiMesh& mesh, Core::Asset::GeometryData& data ) const;

    /// WEIGHTS
    void fetchBoneWeights( const aiMesh& mesh, Core::Asset::GeometryData& data ) const;

    /// MATERIAL
    void loadMaterial( const aiMaterial& material, Core::Asset::GeometryData& data ) const;

  private:
    std::string m_filepath;
};

} // namespace IO
} // namespace Ra

#endif // RADIUMENGINE_ASSIMP_GEOMETRY_DATA_LOADER_HPP
