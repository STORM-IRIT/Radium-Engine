#ifndef RADIUMENGINE_ASSIMP_GEOMETRY_DATA_LOADER_HPP
#define RADIUMENGINE_ASSIMP_GEOMETRY_DATA_LOADER_HPP

#include <assimp/mesh.h>
#include <Core/Log/Log.hpp>
#include <Core/Debug/Loading/AssimpWrapper.hpp>
#include <Core/Debug/Loading/DataLoader.hpp>
#include <Core/Debug/Loading/GeometryData.hpp>

namespace Ra {
namespace Asset {

class AssimpGeometryDataLoader : public DataLoader< GeometryData > {
public:
    /// CONSTRUCTOR
    AssimpGeometryDataLoader( const bool VERBOSE_MODE = false ) : DataLoader< GeometryData >( VERBOSE_MODE ) { }

    /// DESTRUCTOR
    ~AssimpGeometryDataLoader() { }

    /// LOADING
    inline void loadData( const aiScene* scene, std::vector< std::unique_ptr< GeometryData > >& data ) const override {
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
            LOG( logDEBUG ) << "Mesh loaded: " << data.size();
        }
    }

protected:
    /// QUERY
    inline bool sceneHasGeometry( const aiScene* scene ) const {
        return ( sceneGeometrySize( scene ) != 0 );
    }

    uint sceneGeometrySize( const aiScene* scene ) const {
        uint mesh_size = 0;
        if( scene->HasMeshes() ) {
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

    /// LOADING
    inline void loadGeometryData( const aiScene* scene, std::vector< std::unique_ptr< GeometryData > >& data ) const {
        const uint size = scene->mNumMeshes;
        std::map< uint, uint > indexTable;
        for( uint i = 0; i < size; ++i ) {
            aiMesh* mesh = scene->mMeshes[i];
            if( mesh->HasPositions() ) {
                GeometryData* geometry = new GeometryData();
                loadMeshData( *mesh, *geometry );
                if( scene->HasMaterials() ) {
                    const uint matID = mesh->mMaterialIndex;
                    if( matID < scene->mNumMaterials ) {
                        aiMaterial* material = scene->mMaterials[matID];
                        loadMeshMaterial( *material, *geometry );
                    }
                }
                if( m_verbose ) {
                    geometry->displayInfo();
                }
                data.push_back( std::unique_ptr< GeometryData >( geometry ) );
                indexTable[i] = data.size() - 1;
            }
        }
        loadMeshFrame( scene->mRootNode, Core::Transform::Identity(), indexTable, data );
    }


    inline void loadMeshData( const aiMesh& mesh, GeometryData& data ) const {
        fetchName( mesh, data );
        fetchType( mesh, data );
        fetchVertices( mesh, data );
        if( data.isLineMesh() ) {
            fetchEdges( mesh, data );
        } else {
            fetchFaces( mesh, data );
        }
        if( data.isTetraMesh() || data.isHexMesh() ) {
            fetchPolyhedron( mesh, data );
        }
        if( mesh.HasNormals() ) {
            fecthNormals( mesh, data );
        }
        if( mesh.HasTangentsAndBitangents() ) {
            fecthTangents( mesh, data );
            fecthBitangents( mesh, data );
        }
        /*
        if( mesh.HasTextureCoords()) {
            fetchTextureCoordinates( mesh, data );
        }

        if( mesh.HasVertexColors() ) {
            fetchColors( mesh, data );
        }
        */
    }

    inline void loadMeshFrame( const aiNode*                                   node,
                               const Core::Transform&                          parentFrame,
                               const std::map< uint, uint >&                   indexTable,
                               std::vector< std::unique_ptr< GeometryData > >& data ) const {
        const uint child_size = node->mNumChildren;
        const uint mesh_size  = node->mNumMeshes;
        if( ( child_size == 0 ) && ( mesh_size == 0 ) ) {
            return;
        }
        Core::Transform frame = parentFrame * assimpToCore( node->mTransformation );
        for( uint i = 0; i < mesh_size; ++i ) {
            const uint ID = node->mMeshes[i];
            auto it = indexTable.find( ID );
            if( it != indexTable.end() ) {
                data[it->second].setFrame( frame );
            }
        }
        for( uint i = 0; i < child_size; ++i ) {
            loadMeshFrame( node->mChildren[i], frame, indexTable, data );
        }
    }

    /// NAME
    inline void fetchName( const aiMesh& mesh, GeometryData& data ) const {
        data.setName( assimpToCore( mesh.mName ) );
    }

    /// TYPE
    inline void fetchType( const aiMesh& mesh, GeometryData& data ) const {
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

    /// VERTEX
    inline void fetchVertices( const aiMesh& mesh, GeometryData& data ) const {
        const uint size = mesh.mNumVertices;
        std::vector< Core::Vector3 > vertex( size );
        vertex.resize( size );
#pragma omp parallel for
        for( uint i = 0; i < size; ++i ) {
            vertex[i] = assimpToCore( mesh.mVertices[i] );
        }
        data.setVertices( vertex );
    }

    /// EDGE
    inline void fetchEdges( const aiMesh& mesh, GeometryData& data ) const {
        const uint size = mesh.mNumFaces;
        std::vector< Core::Vector2i > edge( size );
#pragma omp parallel for
        for( uint i = 0; i < size; ++i ) {
            edge[i] = assimpToCore( mesh.mFaces[i].mIndices, mesh.mFaces[i].mNumIndices );
        }
        data.setEdges( edge );
    }

    /// FACE
    inline void fetchFaces( const aiMesh& mesh, GeometryData& data ) const {
        const uint size = mesh.mNumFaces;
        std::vector< Core::VectorNi > face( size );
#pragma omp parallel for
        for( uint i = 0; i < size; ++i ) {
            face[i] = assimpToCore( mesh.mFaces[i].mIndices, mesh.mFaces[i].mNumIndices );
        }
        data.setFaces( face );
    }

    /// POLYHEDRON
    inline void fetchPolyhedron( const aiMesh& mesh, GeometryData& data ) const {
        // TO DO
    }

    /// NORMAL
    inline void fetchNormals( const aiMesh& mesh, GeometryData& data ) const {
        const uint size = mesh.mNumVertices;
        std::vector< Core::Vector3 > normal( size );
#pragma omp parallel for
        for( uint i = 0; i < size; ++i ) {
            normal[i] = assimpToCore( mesh.mNormals[i] );
        }
        data.setNormals( normal );
    }

    /// TANGENT
    inline void fetchTangents( const aiMesh& mesh, GeometryData& data ) const {
        const uint size = mesh.mNumVertices;
        std::vector< Core::Vector4 > tangent( size );
#pragma omp parallel for
        for( uint i = 0; i < size; ++i ) {
            Core::Vector3 tmp = assimpToCore( mesh.mTangents[i] );
            tangent[i] = Core::Vector4( tmp[0], tmp[1], tmp[2], 0.0 );
        }
        data.setTangents( tangent );
    }

    /// BITANGENT
    inline void fetchBitangents( const aiMesh& mesh, GeometryData& data ) const {
        const uint size = mesh.mNumVertices;
        std::vector< Core::Vector4 > bitangent( size );
#pragma omp parallel for
        for( uint i = 0; i < size; ++i ) {
            Core::Vector3 tmp = assimpToCore( mesh.mBitangents[i] );
            bitangent[i] = Core::Vector4( tmp[0], tmp[1], tmp[2], 0.0 );
        }
        data.setBitangents( bitangent );
    }

    /// TEXTURE COORDINATE
    inline void fetchTextureCoordinates( const aiMesh& mesh, GeometryData& data ) const {
        // TO DO
    }

    /// COLOR
    inline void fetchColors( const aiMesh& mesh, GeometryData& data ) const {
        // TO DO
    }

    /// MATERIAL
    inline void loadMaterial( const aiMaterial& material, GeometryData& data ) {
        MaterialData mat;
        aiColor4D    color;
        Scalar       shininess;
        aiString     name;
        if( AI_SUCCESS == material->Get( AI_MATKEY_COLOR_DIFFUSE, color ) ) {
            mat.m_hasDiffuse = true;
            mat.m_diffuse    = assimpToCore( color );
        }
        if( AI_SUCCESS == material->Get( AI_MATKEY_COLOR_SPECULAR, color ) ) {
            mat.m_hasSpecular = true;
            mat.m_specular    = assimpToCore( color );
        }
        if( AI_SUCCESS == mat->Get( AI_MATKEY_SHININESS, shininess ) ) {
            mat.m_hasShininess = true;
            mat.m_shininess    = shininess;
        }
        if( AI_SUCCESS == material->Get( AI_MATKEY_TEXTURE( aiTextureType_DIFFUSE, 0 ), name ) ) {
            mat.m_texDiffuse = assimpToCore( name );
        }
        if( AI_SUCCESS == material->Get( AI_MATKEY_TEXTURE( aiTextureType_SPECULAR, 0 ), name ) ) {
            mat.m_texSpecular = assimpToCore( name );
        }
        if( AI_SUCCESS == material->Get( AI_MATKEY_TEXTURE( aiTextureType_SHININESS, 0 ), name ) ) {
            mat.m_texShininess = assimpToCore( name );
        }
        if( AI_SUCCESS == material->Get( AI_MATKEY_TEXTURE( aiTextureType_NORMALS, 0 ), name ) ) {
            mat.m_texNormal = assimpToCore( name );
        }
        if( AI_SUCCESS == material->Get( AI_MATKEY_TEXTURE( aiTextureType_OPACITY, 0 ), name ) ) {
            mat.m_texOpacity = assimpToCore( name );
        }
        data.setMaterial( mat );
    }
};

} // namespace Asset
} // namespace Ra

#endif // RADIUMENGINE_ASSIMP_GEOMETRY_DATA_LOADER_HPP
