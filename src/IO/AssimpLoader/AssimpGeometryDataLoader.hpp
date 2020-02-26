#pragma once

#include <Core/Asset/DataLoader.hpp>
#include <Core/Types.hpp>
#include <IO/RaIO.hpp>

#include <memory>
#include <set>

struct aiScene;
struct aiMesh;
struct aiNode;
struct aiMaterial;

namespace Ra {
namespace Core {
namespace Asset {
class GeometryData;
}
} // namespace Core

namespace IO {

/// The AssimpGeometryDataLoader converts geometry data from the Assimp format
/// to the Asset::GeometryData format.
class RA_IO_API AssimpGeometryDataLoader : public Core::Asset::DataLoader<Core::Asset::GeometryData>
{
  public:
    explicit AssimpGeometryDataLoader( const std::string& filepath,
                                       const bool VERBOSE_MODE = false );

    ~AssimpGeometryDataLoader() override;

    /// Convert all the geometry data from \p scene into \p data.
    void loadData( const aiScene* scene,
                   std::vector<std::unique_ptr<Core::Asset::GeometryData>>& data ) override;

  protected:
    /// Return true if the given scene has geometry data.
    inline bool sceneHasGeometry( const aiScene* scene ) const;

    /// Return the number of AssImp geometry data in the given scene.
    uint sceneGeometrySize( const aiScene* scene ) const;

    /// Fill \p data with all the GeometryData from \p scene.
    void loadGeometryData( const aiScene* scene,
                           std::vector<std::unique_ptr<Core::Asset::GeometryData>>& data );

    /// Fill \p data with the GeometryData from \p mesh.
    void loadMeshData( const aiMesh& mesh,
                       Core::Asset::GeometryData& data,
                       std::set<std::string>& usedNames );

    /// Fill \p data with the Material data from \p material.
    void loadMaterial( const aiMaterial& material, Core::Asset::GeometryData& data ) const;

    /// Fill \p data with the transformation data from \p node and \p parentFrame.
    void loadMeshFrame( const aiNode* node,
                        const Core::Transform& parentFrame,
                        const std::map<uint, size_t>& indexTable,
                        std::vector<std::unique_ptr<Core::Asset::GeometryData>>& data ) const;

    /// Fill \p data with the name from \p mesh.
    /// \note If the name is already in use, then appends as much "_" as needed.
    void fetchName( const aiMesh& mesh,
                    Core::Asset::GeometryData& data,
                    std::set<std::string>& usedNames ) const;

    /// Fill \p data with the GeometryType from \p mesh.
    void fetchType( const aiMesh& mesh, Core::Asset::GeometryData& data ) const;

    /// Fill \p data with the vertices from \p mesh.
    void fetchVertices( const aiMesh& mesh, Core::Asset::GeometryData& data );

    /// Fill \p data with the lines from \p mesh.
    void fetchEdges( const aiMesh& mesh, Core::Asset::GeometryData& data ) const;

    /// Fill \p data with the faces from \p mesh.
    void fetchFaces( const aiMesh& mesh, Core::Asset::GeometryData& data ) const;

    /// Fill \p data with the polyhedra from \p mesh.
    void fetchPolyhedron( const aiMesh& mesh, Core::Asset::GeometryData& data ) const;

    /// Fill \p data with the vertex normals from \p mesh.
    void fetchNormals( const aiMesh& mesh, Core::Asset::GeometryData& data ) const;

    /// Fill \p data with the vertex tangent vectors from \p mesh.
    void fetchTangents( const aiMesh& mesh, Core::Asset::GeometryData& data ) const;

    /// Fill \p data with the vertex bitangent vectors from \p mesh.
    void fetchBitangents( const aiMesh& mesh, Core::Asset::GeometryData& data ) const;

    /// Fill \p data with the vertex texture coordinates from \p mesh.
    void fetchTextureCoordinates( const aiMesh& mesh, Core::Asset::GeometryData& data ) const;

    /// Fill \p data with the vertex colors from \p mesh.
    void fetchColors( const aiMesh& mesh, Core::Asset::GeometryData& data ) const;

  private:
    /// The loaded file path (used to retrieve material texture files).
    std::string m_filepath;
};

} // namespace IO
} // namespace Ra
