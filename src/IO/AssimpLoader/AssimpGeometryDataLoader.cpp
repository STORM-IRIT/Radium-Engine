#include <IO/AssimpLoader/AssimpGeometryDataLoader.hpp>

#include <assimp/mesh.h>
#include <assimp/scene.h>

#include <Core/File/GeometryData.hpp>
#include <Core/Log/Log.hpp>

#include <IO/AssimpLoader/AssimpWrapper.hpp>
#include <Core/File/BlinnPhongMaterialData.hpp>

namespace Ra {
namespace IO {

Triplet::Triplet( const Core::Vector3& v ) : m_v( v ) {}

bool Triplet::operator==( const Triplet& t ) const {
    return ( m_v.isApprox( t.m_v ) );
}

bool Triplet::operator<( const Triplet& t ) const {
    if ( m_v[0] < t.m_v[0] )
    {
        return true;
    }
    if ( m_v[0] == t.m_v[0] )
    {
        if ( m_v[1] < t.m_v[1] )
        {
            return true;
        }
        if ( m_v[1] == t.m_v[1] )
        {
            return ( m_v[2] < t.m_v[2] );
        }
    }
    return false;
}

AssimpGeometryDataLoader::AssimpGeometryDataLoader( const std::string& filepath,
                                                    const bool VERBOSE_MODE ) :
    DataLoader<Asset::GeometryData>( VERBOSE_MODE ),
    m_filepath( filepath ) {}

AssimpGeometryDataLoader::~AssimpGeometryDataLoader() {}

/// LOADING
void AssimpGeometryDataLoader::loadData( const aiScene* scene,
                                         std::vector<std::unique_ptr<Asset::GeometryData>>& data ) {
    data.clear();

    if ( scene == nullptr )
    {
        LOG( logINFO ) << "AssimpGeometryDataLoader : scene is nullptr.";
        return;
    }

    if ( !sceneHasGeometry( scene ) )
    {
        LOG( logINFO ) << "AssimpGeometryDataLoader : scene has no mesh.";
        return;
    }

    if ( m_verbose )
    {
        LOG( logINFO ) << "File contains geometry.";
        LOG( logINFO ) << "Geometry Loading begin...";
    }

    loadGeometryData( scene, data );

    if ( m_verbose )
    {
        LOG( logINFO ) << "Geometry Loading end.\n";
    }
}

bool AssimpGeometryDataLoader::sceneHasGeometry( const aiScene* scene ) const {
    return ( sceneGeometrySize( scene ) != 0 );
}

uint AssimpGeometryDataLoader::sceneGeometrySize( const aiScene* scene ) const {
    uint mesh_size = 0;
    if ( scene->HasMeshes() )
    {
        const uint size = scene->mNumMeshes;
        for ( uint i = 0; i < size; ++i )
        {
            aiMesh* mesh = scene->mMeshes[i];
            if ( mesh->HasPositions() )
            {
                ++mesh_size;
            }
        }
    }
    return mesh_size;
}

void AssimpGeometryDataLoader::loadMeshData( const aiMesh& mesh, Asset::GeometryData& data,
                                             std::set<std::string>& usedNames ) {
    fetchName( mesh, data, usedNames );
    fetchType( mesh, data );
    fetchVertices( mesh, data );
    if ( data.isLineMesh() )
    {
        fetchEdges( mesh, data );
    } else
    { fetchFaces( mesh, data ); }

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
    if ( mesh.HasTextureCoords( 0 ) )
    {
        fetchTextureCoordinates( mesh, data );
    }

    /*
     if( mesh.HasVertexColors() ) {
     fetchColors( mesh, data );
     }
     */

    /*
     if (mesh.HasBones())
     {
     fetchBoneWeights(mesh, data);
     }
     */
}

void AssimpGeometryDataLoader::loadMeshFrame(
    const aiNode* node, const Core::Transform& parentFrame, const std::map<uint, uint>& indexTable,
    std::vector<std::unique_ptr<Asset::GeometryData>>& data ) const {
    const uint child_size = node->mNumChildren;
    const uint mesh_size = node->mNumMeshes;
    if ( ( child_size == 0 ) && ( mesh_size == 0 ) )
    {
        return;
    }

    Core::Transform frame = parentFrame * assimpToCore( node->mTransformation );
    for ( uint i = 0; i < mesh_size; ++i )
    {
        const uint ID = node->mMeshes[i];
        auto it = indexTable.find( ID );
        if ( it != indexTable.end() )
        {
            data[it->second]->setFrame( frame );
        }
    }

    for ( uint i = 0; i < child_size; ++i )
    {
        loadMeshFrame( node->mChildren[i], frame, indexTable, data );
    }
}

void AssimpGeometryDataLoader::fetchName( const aiMesh& mesh, Asset::GeometryData& data,
                                          std::set<std::string>& usedNames ) const {
    std::string name = assimpToCore( mesh.mName );
    while ( usedNames.find( name ) != usedNames.end() )
    {
        name.append( "_" );
    }
    usedNames.insert( name );
    data.setName( name );
}

void AssimpGeometryDataLoader::fetchType( const aiMesh& mesh, Asset::GeometryData& data ) const {
    data.setType( Asset::GeometryData::UNKNOWN );
    uint face_type = 0;
    for ( uint i = 0; i < mesh.mNumFaces; ++i )
    {
        face_type = std::max( face_type, mesh.mFaces[i].mNumIndices );
    }
    if ( face_type != 1 )
    {
        switch ( face_type )
        {
        case 0:
        { data.setType( Asset::GeometryData::POINT_CLOUD ); }
        break;
        case 2:
        { data.setType( Asset::GeometryData::LINE_MESH ); }
        break;
        case 3:
        { data.setType( Asset::GeometryData::TRI_MESH ); }
        break;
        case 4:
        { data.setType( Asset::GeometryData::QUAD_MESH ); }
        break;
        default:
        { data.setType( Asset::GeometryData::POLY_MESH ); }
        break;
        }
    }
}

void AssimpGeometryDataLoader::fetchVertices( const aiMesh& mesh, Asset::GeometryData& data ) {
    const uint size = mesh.mNumVertices;
    auto& vertex = data.getVertices();
    vertex.reserve( size );
    auto& duplicateTable = data.getDuplicateTable();
    duplicateTable.reserve( size );
    std::map<Triplet, uint> uniqueTable;
    for ( uint i = 0; i < size; ++i )
    {
        const Core::Vector3 v = assimpToCore( mesh.mVertices[i] );
        const Triplet t( v );
        auto it = uniqueTable.find( t );
        if ( it == uniqueTable.end() || data.isLoadingDuplicates() )
        {
            duplicateTable.push_back( vertex.size() );
            uniqueTable[t] = vertex.size();
            vertex.push_back( v );
        } else
        { duplicateTable.push_back( it->second ); }
    }
    vertex.shrink_to_fit();
    duplicateTable.shrink_to_fit();
}

/// EDGE
void AssimpGeometryDataLoader::fetchEdges( const aiMesh& mesh, Asset::GeometryData& data ) const {
    const uint size = mesh.mNumFaces;
    auto& edge = data.getEdges();
    edge.resize( size );
#pragma omp parallel for
    for ( uint i = 0; i < size; ++i )
    {
        edge[i] = assimpToCore( mesh.mFaces[i].mIndices, mesh.mFaces[i].mNumIndices ).cast<uint>();
        if ( !data.isLoadingDuplicates() )
        {
            edge[i][0] = data.getDuplicateTable().at( edge[i][0] );
            edge[i][1] = data.getDuplicateTable().at( edge[i][1] );
        }
    }
}

void AssimpGeometryDataLoader::fetchFaces( const aiMesh& mesh, Asset::GeometryData& data ) const {
    const uint size = mesh.mNumFaces;
    auto& face = data.getFaces();
    face.resize( size );
#pragma omp parallel for
    for ( uint i = 0; i < size; ++i )
    {
        face[i] = assimpToCore( mesh.mFaces[i].mIndices, mesh.mFaces[i].mNumIndices ).cast<uint>();
        if ( !data.isLoadingDuplicates() )
        {
            const uint face_vertices = mesh.mFaces[i].mNumIndices;
            for ( uint j = 0; j < face_vertices; ++j )
            {
                face[i][j] = data.getDuplicateTable().at( face[i][j] );
            }
        }
    }
}

void AssimpGeometryDataLoader::fetchPolyhedron( const aiMesh& mesh,
                                                Asset::GeometryData& data ) const {
    // TO DO
}

void AssimpGeometryDataLoader::fetchNormals( const aiMesh& mesh, Asset::GeometryData& data ) const {
    auto& normal = data.getNormals();
    normal.resize( data.getVerticesSize(), Core::Vector3::Zero() );

#pragma omp parallel for if ( data.isLoadingDuplicates() )
    for ( uint i = 0; i < mesh.mNumVertices; ++i )
    {
        normal.at( data.getDuplicateTable().at( i ) ) += assimpToCore( mesh.mNormals[i] );
    }

#pragma omp parallel for
    for ( uint i = 0; i < uint( normal.size() ); ++i )
    {
        normal[i].normalize();
    }
}

void AssimpGeometryDataLoader::fetchTangents( const aiMesh& mesh,
                                              Asset::GeometryData& data ) const {
#if defined( RADIUM_WITH_TEXTURES )
    const uint size = mesh.mNumVertices;
    auto& tangent = data.getTangents();
    tangent.resize( size, Core::Vector3::Zero() );
#    pragma omp parallel for
    for ( uint i = 0; i < size; ++i )
    {
        tangent[i] = assimpToCore( mesh.mTangents[i] );
    }
#endif
}

void AssimpGeometryDataLoader::fetchBitangents( const aiMesh& mesh,
                                                Asset::GeometryData& data ) const {
#if defined( RADIUM_WITH_TEXTURES )
    const uint size = mesh.mNumVertices;
    auto& bitangent = data.getBiTangents();
    bitangent.resize( size );
#    pragma omp parallel for
    for ( uint i = 0; i < size; ++i )
    {
        bitangent[i] = assimpToCore( mesh.mBitangents[i] );
    }
#endif
}

void AssimpGeometryDataLoader::fetchTextureCoordinates( const aiMesh& mesh,
                                                        Asset::GeometryData& data ) const {
#if ( defined( RADIUM_WITH_TEXTURES ) )
    const uint size = mesh.mNumVertices;
    auto& texcoord = data.getTexCoords();
    texcoord.resize( data.getVerticesSize() );
#    pragma omp parallel for
    for ( uint i = 0; i < size; ++i )
    {
        // FIXME(Charly): Is it safe to only consider texcoords[0] ?
        texcoord.at( i ) = assimpToCore( mesh.mTextureCoords[0][i] );
    }
#endif
}

void AssimpGeometryDataLoader::fetchColors( const aiMesh& mesh, Asset::GeometryData& data ) const {
    // TO DO
}

void AssimpGeometryDataLoader::fetchBoneWeights( const aiMesh& mesh,
                                                 Asset::GeometryData& data ) const {
#if 0
            GeometryData::WeightArray weights(data.getVerticesSize());
            
            for (uint i = 0; i < mesh.mNumBones; ++i)
            {
                aiBone* bone = mesh.mBones[i];
                
                for (uint j = 0; j < bone->mNumWeights; ++j)
                {
                    aiVertexWeight w = bone->mWeights[j];
                    
                    uint id = w.mVertexId;
                    CORE_ASSERT(id < data.getVerticesSize(), "Vertex ID is out of bounds");
                    weights[id].push_back({w.mWeight, i});
                }
            }
            
            data.setWeights(weights);
#endif
}

void AssimpGeometryDataLoader::loadMaterial( const aiMaterial& material,
                                             Asset::GeometryData& data ) const {

    std::string matName;
    aiString assimpName;
    if ( AI_SUCCESS == material.Get( AI_MATKEY_NAME, assimpName ) )
    {
        matName = assimpName.C_Str();
    }
    // TODO : use AI_MATKEY_SHADING_MODEL to select the apropriate model
    // (http://assimp.sourceforge.net/lib_html/material_8h.html#a93e23e0201d6ed86fb4287e15218e4cf)
    /*
     aiShadingMode shading;
     if( AI_SUCCESS == material.Get( AI_MATKEY_SHADING_MODEL, shading ) )
     {
     LOG(logINFO) << "Got a "  << shading << " shading model.";
     }
     else
     {
     LOG(logINFO) << "Unable to retrieve shading model.";
     }
     */

    Asset::BlinnPhongMaterialData* blinnPhongMaterial =
        new Asset::BlinnPhongMaterialData( matName );
    aiColor4D color;
    float shininess;
    float opacity;
    aiString name;

    if ( AI_SUCCESS == material.Get( AI_MATKEY_COLOR_DIFFUSE, color ) )
    {
        blinnPhongMaterial->m_hasDiffuse = true;
        blinnPhongMaterial->m_diffuse = assimpToCore( color );
    }

    if ( AI_SUCCESS == material.Get( AI_MATKEY_COLOR_SPECULAR, color ) )
    {
        blinnPhongMaterial->m_hasSpecular = true;
        blinnPhongMaterial->m_specular = assimpToCore( color );
    }

    if ( AI_SUCCESS == material.Get( AI_MATKEY_SHININESS, shininess ) )
    {
        blinnPhongMaterial->m_hasShininess = true;
        blinnPhongMaterial->m_shininess = shininess;
    }

    if ( AI_SUCCESS == material.Get( AI_MATKEY_OPACITY, opacity ) )
    {
        blinnPhongMaterial->m_hasOpacity = true;
        // NOTE(charly): Due to collada way of handling objects that have an alpha map, we must
        // ensure
        //               we do not have zeros in here.
        blinnPhongMaterial->m_opacity = opacity < 1e-5 ? 1 : opacity;
    }

    if ( AI_SUCCESS == material.Get( AI_MATKEY_TEXTURE( aiTextureType_DIFFUSE, 0 ), name ) )
    {
        blinnPhongMaterial->m_texDiffuse = m_filepath + "/" + assimpToCore( name );
        blinnPhongMaterial->m_hasTexDiffuse = true;
    }

    if ( AI_SUCCESS == material.Get( AI_MATKEY_TEXTURE( aiTextureType_SPECULAR, 0 ), name ) )
    {
        blinnPhongMaterial->m_texSpecular = m_filepath + "/" + assimpToCore( name );
        blinnPhongMaterial->m_hasTexSpecular = true;
    }

    if ( AI_SUCCESS == material.Get( AI_MATKEY_TEXTURE( aiTextureType_SHININESS, 0 ), name ) )
    {
        blinnPhongMaterial->m_texShininess = m_filepath + "/" + assimpToCore( name );
        blinnPhongMaterial->m_hasTexShininess = true;
    }

    if ( AI_SUCCESS == material.Get( AI_MATKEY_TEXTURE( aiTextureType_NORMALS, 0 ), name ) )
    {
        blinnPhongMaterial->m_texNormal = m_filepath + "/" + assimpToCore( name );
        blinnPhongMaterial->m_hasTexNormal = true;
    }

    // Assimp loads objs bump maps as height maps, gj bro
    if ( AI_SUCCESS == material.Get( AI_MATKEY_TEXTURE( aiTextureType_HEIGHT, 0 ), name ) )
    {
        blinnPhongMaterial->m_texNormal = m_filepath + "/" + assimpToCore( name );
        blinnPhongMaterial->m_hasTexNormal = true;
    }

    if ( AI_SUCCESS == material.Get( AI_MATKEY_TEXTURE( aiTextureType_OPACITY, 0 ), name ) )
    {
        blinnPhongMaterial->m_texOpacity = m_filepath + "/" + assimpToCore( name );
        blinnPhongMaterial->m_hasTexOpacity = true;
    }

    data.setMaterial( blinnPhongMaterial );
}

void AssimpGeometryDataLoader::loadGeometryData(
    const aiScene* scene, std::vector<std::unique_ptr<Asset::GeometryData>>& data ) {
    const uint size = scene->mNumMeshes;
    std::map<uint, uint> indexTable;
    std::set<std::string> usedNames;
    for ( uint i = 0; i < size; ++i )
    {
        aiMesh* mesh = scene->mMeshes[i];
        if ( mesh->HasPositions() )
        {
            Asset::GeometryData* geometry = new Asset::GeometryData();
#ifdef RADIUM_WITH_TEXTURES
            geometry->setLoadDuplicates( true );
#endif
            loadMeshData( *mesh, *geometry, usedNames );
            if ( scene->HasMaterials() )
            {
                const uint matID = mesh->mMaterialIndex;
                if ( matID < scene->mNumMaterials )
                {
                    aiMaterial* material = scene->mMaterials[matID];
                    loadMaterial( *material, *geometry );
                }
            }
            data.push_back( std::unique_ptr<Asset::GeometryData>( geometry ) );
            indexTable[i] = data.size() - 1;

            if ( m_verbose )
            {
                geometry->displayInfo();
            }
        }
    }
    loadMeshFrame( scene->mRootNode, Core::Transform::Identity(), indexTable, data );
}

} // namespace IO
} // namespace Ra
