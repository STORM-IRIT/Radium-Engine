#ifndef RADIUMENGINE_ASSIMP_GEOMETRY_DATA_LOADER_HPP
#define RADIUMENGINE_ASSIMP_GEOMETRY_DATA_LOADER_HPP

#include <set>

#include <IO/RaIO.hpp>
#include <Core/File/DataLoader.hpp>
#include <Core/Math/LinearAlgebra.hpp>

struct aiScene;
struct aiMesh;
struct aiNode;
struct aiMaterial;

namespace Ra {
namespace Asset {
class GeometryData;
}
}

namespace Ra {
namespace IO {

struct RA_IO_API Triplet {
    Triplet( const Core::Vector3& v = Core::Vector3::Zero() );

    Core::Vector3 m_v;

    bool operator==( const Triplet& t ) const;

    bool operator<( const Triplet& t ) const;
};

class RA_IO_API AssimpGeometryDataLoader : public Asset::DataLoader< Asset::GeometryData > {
public:
    /// CONSTRUCTOR
    AssimpGeometryDataLoader( const std::string& filepath, const bool VERBOSE_MODE = false );

    /// DESTRUCTOR
    ~AssimpGeometryDataLoader();

    /// LOADING
    void loadData( const aiScene* scene, std::vector< std::unique_ptr< Asset::GeometryData > >& data ) override;

protected:
    /// QUERY
    inline bool sceneHasGeometry( const aiScene* scene ) const;

    uint sceneGeometrySize( const aiScene* scene ) const;

    /// LOADING
    void loadGeometryData( const aiScene* scene, std::vector< std::unique_ptr< Asset::GeometryData > >& data );

    void loadMeshData( const aiMesh& mesh, Asset::GeometryData& data , std::set<std::string>& usedNames );

    void loadMeshFrame( const aiNode*                                          node,
                        const Core::Transform&                                 parentFrame,
                        const std::map< uint, uint >&                          indexTable,
                        std::vector< std::unique_ptr< Asset::GeometryData > >& data ) const;

    /// NAME
    void fetchName(const aiMesh& mesh, Asset::GeometryData& data, std::set<std::string>& usedNames) const;

    /// TYPE
    void fetchType( const aiMesh& mesh, Asset::GeometryData& data ) const;

    /// VERTEX
    void fetchVertices( const aiMesh& mesh, Asset::GeometryData& data );

    /// EDGE
    void fetchEdges( const aiMesh& mesh, Asset::GeometryData& data ) const;

    /// FACE
    void fetchFaces( const aiMesh& mesh, Asset::GeometryData& data ) const;

    /// POLYHEDRON
    void fetchPolyhedron( const aiMesh& mesh, Asset::GeometryData& data ) const;

    /// NORMAL
    void fetchNormals( const aiMesh& mesh, Asset::GeometryData& data ) const;

    /// TANGENT
    void fetchTangents( const aiMesh& mesh, Asset::GeometryData& data ) const;

    /// BITANGENT
    void fetchBitangents( const aiMesh& mesh, Asset::GeometryData& data ) const;

    /// TEXTURE COORDINATE
    void fetchTextureCoordinates( const aiMesh& mesh, Asset::GeometryData& data ) const;

    /// COLOR
    void fetchColors( const aiMesh& mesh, Asset::GeometryData& data ) const;

    /// WEIGHTS
    void fetchBoneWeights(const aiMesh& mesh, Asset::GeometryData& data) const;

    /// MATERIAL
    void loadMaterial( const aiMaterial& material, Asset::GeometryData& data ) const;

private:
    std::string m_filepath;
};

} // namespace IO
} // namespace Ra

#endif // RADIUMENGINE_ASSIMP_GEOMETRY_DATA_LOADER_HPP
