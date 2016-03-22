#include <Engine/Assets/AssimpGeometryDataLoader.hpp>

#include <assimp/scene.h>
#include <assimp/mesh.h>

#include <Core/Log/Log.hpp>
#include <Engine/Assets/GeometryData.hpp>
#include <Engine/Assets/AssimpWrapper.hpp>

namespace Ra {
namespace Asset {


    Triplet::Triplet( const Core::Vector3& v ) : m_v( v ) { }

    bool Triplet::operator==( const Triplet& t ) const {
        return ( m_v.isApprox( t.m_v ) );
    }

    bool Triplet::operator<( const Triplet& t ) const {
        if( m_v[0] < t.m_v[0] ) return true;
        if( m_v[0] == t.m_v[0] ) {
            if( m_v[1] < t.m_v[1] ) return true;
            if( m_v[1] == t.m_v[1] ) {
                return ( m_v[2] < t.m_v[2] );
            }
        }
        return false;
    }






AssimpGeometryDataLoader::AssimpGeometryDataLoader( const std::string& filepath, const bool VERBOSE_MODE )
    : DataLoader<GeometryData>( VERBOSE_MODE )
    , m_filepath( filepath )
{
}

AssimpGeometryDataLoader::~AssimpGeometryDataLoader()
{
}

/// LOADING
void AssimpGeometryDataLoader::loadData( const aiScene* scene, std::vector< std::unique_ptr< GeometryData > >& data ) {
    data.clear();

    if( scene == nullptr ) {
        LOG( logDEBUG ) << "AssimpGeometryDataLoader : scene is nullptr.";
        return;
    }

    if( !sceneHasGeometry( scene ) ) {
        LOG( logDEBUG ) << "AssimpGeometryDataLoader : scene has no mesh.";
        return;
    }

    if( m_verbose ) {
        LOG( logDEBUG ) << "File contains geometry.";
        LOG( logDEBUG ) << "Geometry Loading begin...";
    }

    loadGeometryData( scene, data );

    if( m_verbose ) {
        LOG( logDEBUG ) << "Geometry Loading end.\n";
    }
}

bool AssimpGeometryDataLoader::sceneHasGeometry( const aiScene* scene ) const {
    return ( sceneGeometrySize( scene ) != 0 );
}

uint AssimpGeometryDataLoader::sceneGeometrySize( const aiScene* scene ) const {
    uint mesh_size = 0;
    if( scene->HasMeshes() )
    {
        const uint size = scene->mNumMeshes;
        for( uint i = 0; i < size; ++i ) {
            aiMesh* mesh = scene->mMeshes[i];
            if( mesh->HasPositions() ) {
                ++mesh_size;
            }
        }
    }
    return mesh_size;
}

void AssimpGeometryDataLoader::loadMeshData( const aiMesh& mesh, GeometryData& data ) {
    fetchName( mesh, data );
    fetchType( mesh, data );
    fetchVertices( mesh, data );
    if ( data.isLineMesh() )
    {
        fetchEdges( mesh, data );
    }
    else
    {
        fetchFaces( mesh, data );
    }

    if ( data.isTetraMesh() || data.isHexMesh() )
    {
        fetchPolyhedron( mesh, data );
    }

    if ( mesh.HasNormals() )
    {
        fetchNormals( mesh, data );
    }

    if ( mesh.HasTangentsAndBitangents() )
    {
        fetchTangents( mesh, data );
        fetchBitangents( mesh, data );
    }

    // FIXME( Charly ) << "Is it safe to only consider texcoord 0 ?
    if( mesh.HasTextureCoords( 0 ) ) {
        fetchTextureCoordinates( mesh, data );
    }

    /*
    if( mesh.HasVertexColors() ) {
        fetchColors( mesh, data );
    }
    */
}

void AssimpGeometryDataLoader::loadMeshFrame( const aiNode* node, const Core::Transform& parentFrame,
                                              const std::map< uint, uint >& indexTable,
                                              std::vector< std::unique_ptr< GeometryData > >& data ) const
{
    const uint child_size = node->mNumChildren;
    const uint mesh_size  = node->mNumMeshes;
    if( ( child_size == 0 ) && ( mesh_size == 0 ) )
    {
        return;
    }

    Core::Transform frame = parentFrame * assimpToCore( node->mTransformation );
    for( uint i = 0; i < mesh_size; ++i )
    {
        const uint ID = node->mMeshes[i];
        auto it = indexTable.find( ID );
        if( it != indexTable.end() )
        {
            data[it->second]->setFrame( frame );
        }
    }

    for( uint i = 0; i < child_size; ++i )
    {
        loadMeshFrame( node->mChildren[i], frame, indexTable, data );
    }
}

void AssimpGeometryDataLoader::fetchName( const aiMesh& mesh, GeometryData& data ) const
{
    data.setName( assimpToCore( mesh.mName ) );
}

void AssimpGeometryDataLoader::fetchType( const aiMesh& mesh, GeometryData& data ) const
{
    data.setType( GeometryData::UNKNOWN );
    uint face_type = 0;
    for( uint i = 0; i < mesh.mNumFaces; ++i ) {
        face_type = std::max( face_type, mesh.mFaces[i].mNumIndices );
    }
    if( face_type != 1 ) {
        switch( face_type ) {
            case 0: {
                data.setType( GeometryData::POINT_CLOUD );
            } break;
            case 2: {
                data.setType( GeometryData::LINE_MESH );
            } break;
            case 3: {
                data.setType( GeometryData::TRI_MESH );
            } break;
            case 4: {
                data.setType( GeometryData::QUAD_MESH );
            } break;
            default: {
                data.setType( GeometryData::POLY_MESH );
            } break;
        }
    }
}

void AssimpGeometryDataLoader::fetchVertices( const aiMesh& mesh, GeometryData& data ) {
    const uint size = mesh.mNumVertices;
#if 0
    std::vector< Core::Vector3 > vertex( size );
    vertex.resize( size );
#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        vertex[i] = assimpToCore( mesh.mVertices[i] );
    }
#else
    std::vector< Core::Vector3 > vertex;
    std::map< Triplet, uint > uniqueTable;
    data.m_duplicateTable.clear();
    for( uint i = 0; i < size; ++i ) {
        const Core::Vector3 v = assimpToCore( mesh.mVertices[i] );
        const Triplet t( v );
        auto it = uniqueTable.find( t );
        if( ( it == uniqueTable.end() ) || data.isLoadingDuplicates() ) {
            vertex.push_back( v );
            uniqueTable[t]           = vertex.size() - 1;
            data.m_duplicateTable[i] = vertex.size() - 1;
        } else {
            data.m_duplicateTable[i] = it->second;
        }
    }
#endif
    data.setVertices( vertex );
}

/// EDGE
void AssimpGeometryDataLoader::fetchEdges( const aiMesh& mesh, GeometryData& data ) const {
    const uint size = mesh.mNumFaces;
    std::vector< Core::Vector2ui > edge( size );
#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        edge[i] = assimpToCore( mesh.mFaces[i].mIndices, mesh.mFaces[i].mNumIndices ).cast<uint>();
        edge[i][0] = data.m_duplicateTable.at( edge[i][0] );
        edge[i][1] = data.m_duplicateTable.at( edge[i][1] );
    }
    data.setEdges( edge );
}

void AssimpGeometryDataLoader::fetchFaces( const aiMesh& mesh, GeometryData& data ) const {
    const uint size = mesh.mNumFaces;
    std::vector< Core::VectorNui > face( size );
#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        face[i] = assimpToCore( mesh.mFaces[i].mIndices, mesh.mFaces[i].mNumIndices ).cast<uint>();
        const uint face_vertices = mesh.mFaces[i].mNumIndices;
        for( uint j = 0; j < face_vertices; ++j ) {
            face[i][j] = data.m_duplicateTable.at( face[i][j] );
        }
    }
    data.setFaces( face );
}

void AssimpGeometryDataLoader::fetchPolyhedron( const aiMesh& mesh, GeometryData& data ) const {
    // TO DO
}

void AssimpGeometryDataLoader::fetchNormals( const aiMesh& mesh, GeometryData& data ) const {
    const uint size = mesh.mNumVertices;
    std::vector< Core::Vector3 > normal( data.getVerticesSize(), Core::Vector3::Zero() );
#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        normal.at( data.m_duplicateTable.at( i ) ) += assimpToCore( mesh.mNormals[i] );
    }
#pragma omp parallel for
    for( uint i = 0; i < normal.size(); ++i ) {
        normal[i].normalize();
    }
    data.setNormals( normal );
}

void AssimpGeometryDataLoader::fetchTangents( const aiMesh& mesh, GeometryData& data ) const {
#ifdef NORMAL_MAPPING_IS_IMPLEMENTED_CORRECTLY
    const uint size = mesh.mNumVertices;
    std::vector< Core::Vector4 > tangent( data.getVerticesSize() );
#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        Core::Vector3 tmp = assimpToCore( mesh.mTangents[i] );
        tangent[i] = Core::Vector4( tmp[0], tmp[1], tmp[2], 0.0 );
    }
    data.setTangents( tangent );
#endif
}

void AssimpGeometryDataLoader::fetchBitangents( const aiMesh& mesh, GeometryData& data ) const {
#ifdef NORMAL_MAPPING_IS_IMPLEMENTED_CORRECTLY
    const uint size = mesh.mNumVertices;
    std::vector< Core::Vector4 > bitangent( data.getVerticesSize() );
#pragma omp parallel for
    for( uint i = 0; i < size; ++i ) {
        Core::Vector3 tmp = assimpToCore( mesh.mBitangents[i] );
        bitangent[i] = Core::Vector4( tmp[0], tmp[1], tmp[2], 0.0 );
    }
    data.setBitangents( bitangent );
#endif
}

void AssimpGeometryDataLoader::fetchTextureCoordinates( const aiMesh& mesh, GeometryData& data ) const {
#if defined(TEXTURE_MAPPING_IS_IMPLEMENTED_CORRECTLY) or defined(LOAD_TEXTURES)
    const uint size = mesh.mNumVertices;
    std::vector<Core::Vector3> texcoord;
    texcoord.resize(data.getVerticesSize());
#pragma omp parallel for
    for ( uint i = 0; i < size; ++i )
    {
        // FIXME(Charly): Is it safe to only consider texcoords[0] ?
        // FIXME(Charly): This is probably crappy if you do not allow duplicates.
        texcoord.at(data.m_duplicateTable.at(i)) = assimpToCore( mesh.mTextureCoords[0][i] );
    }
    data.setTextureCoordinates( texcoord );
#endif
}

void AssimpGeometryDataLoader::fetchColors( const aiMesh& mesh, GeometryData& data ) const {
    // TO DO
}

void AssimpGeometryDataLoader::loadMaterial( const aiMaterial& material, GeometryData& data ) const {

    MaterialData mat;
    aiColor4D    color;
    float        shininess;
    aiString     name;

    if( AI_SUCCESS == material.Get( AI_MATKEY_COLOR_DIFFUSE, color ) )
    {
        mat.m_hasDiffuse = true;
        mat.m_diffuse    = assimpToCore( color );
    }

    if( AI_SUCCESS == material.Get( AI_MATKEY_COLOR_SPECULAR, color ) )
    {
        mat.m_hasSpecular = true;
        mat.m_specular    = assimpToCore( color );
    }

    if( AI_SUCCESS == material.Get( AI_MATKEY_SHININESS, shininess ) )
    {
        mat.m_hasShininess = true;
        mat.m_shininess    = shininess;
    }

    if( AI_SUCCESS == material.Get( AI_MATKEY_TEXTURE( aiTextureType_DIFFUSE, 0 ), name ) )
    {
        mat.m_texDiffuse = m_filepath + "/" + assimpToCore( name );
    }

    if( AI_SUCCESS == material.Get( AI_MATKEY_TEXTURE( aiTextureType_SPECULAR, 0 ), name ) )
    {
        mat.m_texSpecular = m_filepath + "/" + assimpToCore( name );
    }

    if( AI_SUCCESS == material.Get( AI_MATKEY_TEXTURE( aiTextureType_SHININESS, 0 ), name ) )
    {
        mat.m_texShininess = m_filepath + "/" + assimpToCore( name );
    }

    if( AI_SUCCESS == material.Get( AI_MATKEY_TEXTURE( aiTextureType_NORMALS, 0 ), name ) )
    {
        mat.m_texNormal = m_filepath + "/" + assimpToCore( name );
    }

    // Assimp loads objs bump maps as height maps, gj bro
    if( AI_SUCCESS == material.Get( AI_MATKEY_TEXTURE( aiTextureType_HEIGHT, 0 ), name ) )
    {
        mat.m_texNormal = m_filepath + "/" + assimpToCore( name );
    }

    if( AI_SUCCESS == material.Get( AI_MATKEY_TEXTURE( aiTextureType_OPACITY, 0 ), name ) )
    {
        mat.m_texOpacity = m_filepath + "/" + assimpToCore( name );
    }

    data.setMaterial( mat );
}

void AssimpGeometryDataLoader::loadGeometryData( const aiScene* scene, std::vector< std::unique_ptr< GeometryData > >& data ) {
    const uint size = scene->mNumMeshes;
    std::map< uint, uint > indexTable;
    for( uint i = 0; i < size; ++i ) {
        aiMesh* mesh = scene->mMeshes[i];
        if( mesh->HasPositions() ) {
            GeometryData* geometry = new GeometryData();
#ifdef LOAD_TEXTURES
            geometry->setLoadDuplicates(true);
#endif
            loadMeshData( *mesh, *geometry );
            if( scene->HasMaterials() ) {
                const uint matID = mesh->mMaterialIndex;
                if( matID < scene->mNumMaterials ) {
                    aiMaterial* material = scene->mMaterials[matID];
                    loadMaterial( *material, *geometry );
                }
            }
            data.push_back( std::unique_ptr< GeometryData >( geometry ) );
            indexTable[i] = data.size() - 1;

            if ( m_verbose )
            {
                geometry->displayInfo();
            }

        }
    }
    loadMeshFrame( scene->mRootNode, Core::Transform::Identity(), indexTable, data );
}



} // namespace Asset
} // namespace Ra
