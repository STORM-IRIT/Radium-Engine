
#include <Engine/Assets/AssimpHandleDataLoader.hpp>

#include <set>
#include <Core/Log/Log.hpp>
#include <Engine/Assets/AssimpWrapper.hpp>
#include <Engine/Assets/HandleData.hpp>

// FIXME(Charly): Needs to be fixed to be compiled
#ifdef DEBUG_LOAD_HANDLE

namespace Ra {
namespace Asset {

/// CONSTRUCTOR
AssimpHandleDataLoader::AssimpHandleDataLoader( const bool VERBOSE_MODE ) : DataLoader< HandleData > ( VERBOSE_MODE ) { }

/// DESTRUCTOR
AssimpHandleDataLoader::~AssimpHandleDataLoader() { }

/// LOAD
void AssimpHandleDataLoader::loadData( const aiScene* scene, std::vector< std::unique_ptr< HandleData > >& data ) const {
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
    }
}



/// QUERY
bool AssimpHandleDataLoader::sceneHasHandle( const aiScene* scene ) const {
    return ( sceneHandleSize( scene ) != 0 );
}

uint AssimpHandleDataLoader::sceneHandleSize( const aiScene* scene ) const {
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
void AssimpHandleDataLoader::loadHandleData( const aiScene* scene, std::vector< std::unique_ptr< HandleData > >& data ) const {
    const uint meshSize = scene->mNumMeshes;
    std::map< uint, uint > indexTable;
    for( uint n = 0; n < meshSize; ++n ) {
        aiMesh* mesh = scene->mMeshes[n];
        if( mesh->HasBones() ) {
            HandleData* handle = new HandleData();
            fetchName( *mesh, *handle );
            fetchType( *mesh, *handle );
            loadHandleComponentData( scene, mesh, handle );
            loadHandleTopologyData( scene, handle );
            fetchVertexSize( *handle );
            data.push_back( std::unique_ptr< HandleData >( handle ) );
            indexTable[n] = data.size() - 1;
            if( m_verbose ) {
                handle->displayInfo();
            }
        }
    }
    loadHandleFrame( scene->mRootNode, Core::Transform::Identity(), indexTable, data );
}

void AssimpHandleDataLoader::loadHandleComponentData( const aiScene* scene, const aiMesh* mesh, HandleData* data ) const {
    const uint size = mesh->mNumBones;
    std::vector< HandleComponentData > component( size, HandleComponentData() );
    std::set< std::string > name;
    std::map< std::string , uint > nameTable;
    // Load the meaningful handles

    for( uint i = 0; i < size; ++i ) {
        aiBone* bone = mesh->mBones[i];
        loadHandleComponentData( scene, bone, component[i] );
        nameTable[component[i].m_name] = i;
        name.insert( component[i].m_name );
    }
    // Load the meaningless handles (if any)
    for( uint i = 0; i < size; ++i ) {
        aiNode* node = scene->mRootNode->FindNode( mesh->mBones[i]->mName );
        const uint children_size = node->mNumChildren;
        if( ( children_size == 0 ) && ( component[i].m_weight.size() != 0 ) ) {
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

void AssimpHandleDataLoader::loadHandleComponentData( const aiScene* scene, const aiBone* bone, HandleComponentData& data ) const {
    data.m_name  = assimpToCore( bone->mName );
    data.m_frame = assimpToCore( bone->mOffsetMatrix );
    const uint size = bone->mNumWeights;
    for( uint j = 0; j < size; ++j ) {
        std::pair< uint, Scalar > weight( bone->mWeights[j].mVertexId, bone->mWeights[j].mWeight );
        data.m_weight.push_back( weight );
    }
    aiNode* node = scene->mRootNode->FindNode( bone->mName );
    while( node != nullptr ) {
        data.m_frame = data.m_frame * assimpToCore( node->mTransformation );
        node = node->mParent;
    }
}

void AssimpHandleDataLoader::loadHandleComponentData( const aiNode* node, HandleComponentData& data ) const {
    data.m_name  = assimpToCore( node->mName );
    data.m_frame = assimpToCore( node->mTransformation );
    const aiNode* tmpNode = node;
    while( tmpNode != nullptr ) {
        data.m_frame = data.m_frame * assimpToCore( node->mTransformation );
        tmpNode = tmpNode->mParent;
    }
}

void AssimpHandleDataLoader::loadHandleTopologyData( const aiScene* scene, HandleData* data ) const {
    const uint size = data->getComponentDataSize();
    std::vector< std::pair< std::string, std::string > > edgeList;
    for( uint i = 0; i < size; ++i ) {
        HandleComponentData& component = data->m_component[i];
        aiNode* node = scene->mRootNode->FindNode( aiString( component.m_name ) );
        const uint children_size = node->mNumChildren;
        for( uint j = 0; j < children_size; ++j ) {
            std::string childName = assimpToCore( node->mChildren[j]->mName );
            if( data->getIndexOf( childName ) != -1 ) {
                edgeList.push_back( std::pair< std::string, std::string >( component.m_name, childName ) );
            }
        }
    }
    std::vector< Core::Vector2i > edge;
    edge.reserve( edgeList.size() );
    for( const auto& it : edgeList ) {
        Core::Vector2i e;
        e[0] = data->getIndexOf( it.first  );
        e[1] = data->getIndexOf( it.second );
        edge.push_back( e );
    }
    data->setEdges( edge );
}

void AssimpHandleDataLoader::loadHandleFrame( const aiNode*                                 node,
                                              const Core::Transform&                        parentFrame,
                                              const std::map< uint, uint >&                 indexTable,
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
            data[it->second]->setFrame( frame );
        }
    }
    for( uint i = 0; i < child_size; ++i ) {
        loadHandleFrame( node->mChildren[i], frame, indexTable, data );
    }
}

/// NAME
void AssimpHandleDataLoader::fetchName( const aiMesh& mesh, HandleData& data ) const {
    data.setName( assimpToCore( mesh.mName ) );
}

/// TYPE
void AssimpHandleDataLoader::fetchType( const aiMesh& mesh, HandleData& data ) const {
    data.setType( HandleData::SKELETON );
    // TO DO: is there a way to know the right type of handle?
}

/// VERTEX SIZE
void AssimpHandleDataLoader::fetchVertexSize( HandleData& data ) const {
    const uint componentSize = data.getComponentDataSize();
    data.m_vertexSize = 0;
    for( uint i = 0; i < componentSize; ++i ) {
        for( const auto& item : data.m_component[i].m_weight ) {
            data.m_vertexSize = std::max( data.m_vertexSize, ( item.first + 1 ) );
        }
    }
}
#endif

} // namespace Asset
} // namespace Ra
