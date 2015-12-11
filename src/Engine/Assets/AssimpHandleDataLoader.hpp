#ifndef RADIUMENGINE_ASSIMP_HANDLE_DATA_LOADER_HPP
#define RADIUMENGINE_ASSIMP_HANDLE_DATA_LOADER_HPP

#include <map>
#include <set>
#include <assimp/mesh.h>
#include <Core/Log/Log.hpp>
#include <Core/Debug/Loading/AssimpWrapper.hpp>
#include <Core/Debug/Loading/DataLoader.hpp>
#include <Core/Debug/Loading/HandleData.hpp>

namespace Ra {
namespace Asset {

class AssimpHandleDataLoader : public DataLoader< HandleData > {
public:
    /// CONSTRUCTOR
    AssimpHandleDataLoader( const bool VERBOSE_MODE = false ) : DataLoader< HandleData > ( VERBOSE_MODE ) { }

    /// DESTRUCTOR
    ~AssimpHandleDataLoader() { }

    /// LOAD
    inline void loadData( const aiScene* scene, std::vector< std::unique_ptr< HandleData > >& data ) const override {
        data.clear();

        if( scene == nullptr ) {
            LOG( logDEBUG ) << "AssimpHandleDataLoader : scene is nullptr.";
            return;
        }

        if( !sceneHasHandle( scene ) ) {
            LOG( logDEBUG ) << "AssimpHandleDataLoader : scene has no handle.";
            return;
        }

        if( m_verbose ) {
            LOG( logDEBUG ) << "File contains handle.";
            LOG( logDEBUG ) << "Handle Loading begin...";
        }

        loadHandleData( scene, data );

        if( m_verbose ) {
            LOG( logDEBUG ) << "Handle Loading end.\n";
            LOG( logDEBUG ) << "Handle loaded: " << data.size();
        }
    }



protected:
    /// QUERY
    inline bool sceneHasHandle( const aiScene* scene ) const {
        return ( sceneHandleSize( scene ) != 0 );
    }

    uint sceneHandleSize( const aiScene* scene ) const {
        const uint size = scene->mNumMeshes;
        uint handle_size = 0;
        if( scene != 0 ) {
            for( uint i = 0; i < size; ++i ) {
                aiMesh* mesh = scene->mMeshes[i];
                if( mesh->HasBones() ) {
                    ++handle_size;
                }
            }
        }
        return handle_size;
    }



    /// LOAD
    inline void loadHandleData( const aiScene* scene, std::vector< std::unique_ptr< HandleData > >& data ) {
        const uint meshSize = scene->mNumMeshes;
        std::map< uint, uint > indexTable;
        for( uint n = 0; n < meshSize; ++n ) {
            aiMesh* mesh = scene->mMeshes[n];
            if( mesh->HasBones() ) {
                HandleData* handle = new HandleData();
                fetchName( *mesh, *handle );
                fetchType( *mesh, *handle );
                loadHandleComponentData( scene, mesh, handle );
                loadHandleTopology( scene, handle );

                data.push_back( std::unique_ptr< HandleData >( handle ) );
                indexTable[n] = data.size() - 1;
                if( m_verbose ) {
                    handle->displayInfo();
                }
            }
        }
        loadHandleFrame( scene->mRootNode, Core::Transform::Identity(), indexTable, data );
    }



    inline void loadHandleComponentData( aiScene* scene, aiMesh* mesh, HandleData* data ) {
        const uint size = mesh->mNumBones;
        std::vector< HandleComponentData > component( size, HandleComponentData() );
        std::set< std::string > name;
        std::map< std::string , uint > nameTable;
        // Load the meaningful handles
        for( uint i = 0; i < size; ++i ) {
            aiBone* bone = mesh->mBones[i];
            loadHandleComponentData( bone, component[i] );
            nameTable[component[i].m_name] = i;
            name.insert( component[i].m_name );
        }
        // Load the meaningless handles (if any)
        for( uint i = 0; i < size; ++i ) {
            aiNode* node = scene->mRootNode->FindNode( mesh->mBones[i]->mName );
            const uint children_size = node->mNumChildren;
            if( children_size == 0 ) {
                data->m_endNode = true;
            } else {
                if( children_size == 1 ) {
                    aiNode* child = node->mChildren[0];
                    auto it = name.find( assimpToCore( child->mName ) );
                    if( it == name.end() ) {
                        component.push_back( HandleComponentData() );
                        const uint j = component.size() - 1;
                        loadHandleComponentData( child, component[j] );
                        nameTable[component[j].m_name] = j;
                    }
                }
            }
        }
        data->setComponents( component );
        data->setNameTable( nameTable );
    }



    inline void loadHandleComponentData( aiBone* bone, HandleComponentData& data ) {
        data.m_name  = assimpToCore( bone->mName );
        data.m_frame = assimpToCore( bone->mOffsetMatrix );
        const uint size = bone->mNumWeights;
        for( uint j = 0; j < size; ++j ) {
            std::pair< uint, Scalar > w( bone->mWeights[j].mVertexId, bone->mWeights[j].mWeight );
            data.m_weight.push_back( w );
        }
    }



    inline void loadHandleComponentData( aiNode* node, HandleComponentData& data ) {
        data.m_name  = assimpToCore( node->mName );
        data.m_frame = assimpToCore( node->mTransformation );
    }



    inline void loadHandleTopologyData( aiScene* scene, HandleData* data ) {
        const uint size = data->getComponentDataSize();
        std::map< std::string, std::string > edgeTable;
        for( uint i = 0; i < size; ++i ) {
            HandleComponentData& component = data->m_component[i];
            aiNode* node = scene->mRootNode->FindNode( aiString( component.m_name ) );
            const uint children_size = node->mNumChildren;
            for( uint j = 0; j < children_size; ++j ) {
                std::string childName = assimpToCore( node->mChildren[j]->mName );
                if( data->getIndexOf( childName ) != -1 ) {
                    edgeTable[ component.m_name ] = childName;
                }
            }
        }
        std::vector< Core::Vector2i > edge( edgeTable.size(), Core::Vector2i::Zero() );
        for( const auto& it : edgeTable ) {
            edge[i][0] = data->getIndexOf( it->first  );
            edge[i][1] = data->getIndexOf( it->second );
        }
        data->setEdges( edge );
    }



    inline void loadHandleFrame( aiNode*                                         node,
                                 const Core::Transform&                          parentFrame,
                                 const std::map< uint, uint >&                   indexTable,
                                 std::vector< std::unique_ptr< HandleData > >& data ) const {
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
            loadHandleFrame( node->mChildren[i], frame, indexTable, data );
        }
    }

    /// NAME
    inline void fetchName( const aiMesh& mesh, HandleData& data ) const {
        data.setName( assimpToCore( mesh.mName ) );
    }

    /// TYPE
    inline void fetchType( const aiMesh& mesh, HandleData& data ) const {
        data.setType( HandleData::SKELETON );
        // TO DO: is there a way to know the right type of handle?
    }

};



} // namespace Asset
} // namespace Ra

#endif // RADIUMENGINE_ASSIMP_HANDLE_DATA_LOADER_HPP
