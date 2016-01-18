#ifndef RADIUMENGINE_ASSIMP_GEOMETRY_DATA_LOADER_HPP
#define RADIUMENGINE_ASSIMP_GEOMETRY_DATA_LOADER_HPP

#include <assimp/mesh.h>

#include <Core/Math/LinearAlgebra.hpp>
#include <Engine/Assets/DataLoader.hpp>

namespace Ra {
namespace Asset {

class GeometryData;

struct Triplet {
    Triplet( const Core::Vector3& v = Core::Vector3::Zero() );

    Core::Vector3 m_v;

    bool operator==( const Triplet& t ) const;

    bool operator<( const Triplet& t ) const;
};

class AssimpGeometryDataLoader : public DataLoader< GeometryData > {
public:
    /// CONSTRUCTOR
    AssimpGeometryDataLoader( const std::string& filepath, const bool VERBOSE_MODE = false );

    /// DESTRUCTOR
    ~AssimpGeometryDataLoader();

    /// LOADING
    void loadData( const aiScene* scene, std::vector< std::unique_ptr< GeometryData > >& data ) override;

protected:
    /// QUERY
    inline bool sceneHasGeometry( const aiScene* scene ) const;

    uint sceneGeometrySize( const aiScene* scene ) const;

    /// LOADING
    void loadGeometryData( const aiScene* scene, std::vector< std::unique_ptr< GeometryData > >& data );

    void loadMeshData( const aiMesh& mesh, GeometryData& data );

    void loadMeshFrame( const aiNode*                                   node,
                        const Core::Transform&                          parentFrame,
                        const std::map< uint, uint >&                   indexTable,
                        std::vector< std::unique_ptr< GeometryData > >& data ) const;

    /// NAME
    void fetchName( const aiMesh& mesh, GeometryData& data ) const;

    /// TYPE
    void fetchType( const aiMesh& mesh, GeometryData& data ) const;

    /// VERTEX
    void fetchVertices( const aiMesh& mesh, GeometryData& data );

    /// EDGE
    void fetchEdges( const aiMesh& mesh, GeometryData& data ) const;

    /// FACE
    void fetchFaces( const aiMesh& mesh, GeometryData& data ) const;

    /// POLYHEDRON
    void fetchPolyhedron( const aiMesh& mesh, GeometryData& data ) const;

    /// NORMAL
    void fetchNormals( const aiMesh& mesh, GeometryData& data ) const;

    /// TANGENT
    void fetchTangents( const aiMesh& mesh, GeometryData& data ) const;

    /// BITANGENT
    void fetchBitangents( const aiMesh& mesh, GeometryData& data ) const;

    /// TEXTURE COORDINATE
    void fetchTextureCoordinates( const aiMesh& mesh, GeometryData& data ) const;

    /// COLOR
    void fetchColors( const aiMesh& mesh, GeometryData& data ) const;

    /// MATERIAL
    void loadMaterial( const aiMaterial& material, GeometryData& data ) const;

private:
    std::string m_filepath;
};

} // namespace Asset
} // namespace Ra

#endif // RADIUMENGINE_ASSIMP_GEOMETRY_DATA_LOADER_HPP
