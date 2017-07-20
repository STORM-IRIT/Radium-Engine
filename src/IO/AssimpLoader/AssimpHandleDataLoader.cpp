#include <IO/AssimpLoader/AssimpHandleDataLoader.hpp>

#include <set>
#include <assimp/scene.h>
#include <assimp/mesh.h>

#include <Core/Containers/AlignedStdVector.hpp>
#include <Core/File/HandleData.hpp>
#include <Core/Log/Log.hpp>

#include <IO/AssimpLoader/AssimpWrapper.hpp>

namespace Ra {
namespace IO {

/// CONSTRUCTOR
AssimpHandleDataLoader::AssimpHandleDataLoader( const bool VERBOSE_MODE ) : Asset::DataLoader< Asset::HandleData > ( VERBOSE_MODE ) { }

/// DESTRUCTOR
AssimpHandleDataLoader::~AssimpHandleDataLoader() { }

/// LOAD
void AssimpHandleDataLoader::loadData( const aiScene* scene, std::vector< std::unique_ptr< Asset::HandleData > >& data ) {
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
void AssimpHandleDataLoader::loadHandleData( const aiScene* scene, std::vector< std::unique_ptr< Asset::HandleData > >& data ) const {
    const uint meshSize = scene->mNumMeshes;
    std::map< uint, uint > indexTable;
    std::set<std::string> usedNames;
    for( uint n = 0; n < meshSize; ++n ) {
        aiMesh* mesh = scene->mMeshes[n];
        if( mesh->HasBones() ) {
            Asset::HandleData* handle = new Asset::HandleData();
            fetchName( *mesh, *handle, usedNames);
            fetchType( *mesh, *handle );
            loadHandleComponentData( scene, mesh, handle );
            loadHandleTopologyData( scene, handle );
            fetchVertexSize( *handle );

            // Remove scale from transform.
            for( auto& component : handle->getComponentData() ) {
                Core::Transform& frame = component.m_frame;
                Core::Vector3 t = frame.translation();
                Core::Matrix3 R = frame.rotation();
                frame.setIdentity();
                frame.translation() = t;
                frame.linear() = R;
            }

            data.push_back( std::unique_ptr< Asset::HandleData >( handle ) );
            indexTable[n] = data.size() - 1;
            if( m_verbose ) {
                handle->displayInfo();
            }
        }
    }
    loadHandleFrame( scene->mRootNode, Core::Transform::Identity(), indexTable, data );
}

void AssimpHandleDataLoader::loadHandleComponentData( const aiScene* scene, const aiMesh* mesh, Asset::HandleData* data ) const {
    const uint size = mesh->mNumBones;
    Core::AlignedStdVector< Asset::HandleComponentData > component( size, Asset::HandleComponentData() );
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
            data->needEndNodes( true );
        } else {
            if( children_size == 1 ) {
                aiNode* child = node->mChildren[0];
                auto it = name.find( assimpToCore( child->mName ) );
                if( it == name.end() ) {
                    component.push_back( Asset::HandleComponentData() );
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

void AssimpHandleDataLoader::loadHandleComponentData( const aiScene* scene, const aiBone* bone, Asset::HandleComponentData& data ) const {
    data.m_name  = assimpToCore( bone->mName );
    //data.m_frame = assimpToCore( bone->mOffsetMatrix );
    data.m_frame.setIdentity();
    const uint size = bone->mNumWeights;
    for( uint j = 0; j < size; ++j ) {
        std::pair< uint, Scalar > weight( bone->mWeights[j].mVertexId, bone->mWeights[j].mWeight );
        data.m_weight.push_back( weight );
    }
    aiNode* node = scene->mRootNode->FindNode( bone->mName );
    while( node != nullptr ) {
        data.m_frame = assimpToCore( node->mTransformation ) * data.m_frame;
        node = node->mParent;
    }
}

void AssimpHandleDataLoader::loadHandleComponentData( const aiNode* node, Asset::HandleComponentData& data ) const {
    data.m_name  = assimpToCore( node->mName );
    //data.m_frame = assimpToCore( node->mTransformation );
    data.m_frame.setIdentity();
    const aiNode* tmpNode = node;
    while( tmpNode != nullptr ) {
        data.m_frame = assimpToCore( tmpNode->mTransformation ) * data.m_frame;
        tmpNode = tmpNode->mParent;
    }
}

void AssimpHandleDataLoader::loadHandleTopologyData( const aiScene* scene, Asset::HandleData* data ) const {
    const uint size = data->getComponentDataSize();
    std::vector< std::pair< std::string, std::string > > edgeList;
    for( uint i = 0; i < size; ++i ) {
        const Asset::HandleComponentData& component = data->getComponent(i);
        aiNode* node = scene->mRootNode->FindNode( aiString( component.m_name ) );
        const uint children_size = node->mNumChildren;
        for( uint j = 0; j < children_size; ++j ) {
            std::string childName = assimpToCore( node->mChildren[j]->mName );
            if( data->getIndexOf( childName ) != -1 ) {
                edgeList.push_back( std::pair< std::string, std::string >( component.m_name, childName ) );
            }
        }
    }
    Core::AlignedStdVector< Core::Vector2i > edge;
    edge.reserve( edgeList.size() );
    for( const auto& it : edgeList ) {
        Core::Vector2i e;
        e[0] = data->getIndexOf( it.first  );
        e[1] = data->getIndexOf( it.second );
        edge.push_back( e );
    }
    data->setEdges( edge );
}

void AssimpHandleDataLoader::loadHandleFrame( const aiNode*                                        node,
                                              const Core::Transform&                               parentFrame,
                                              const std::map< uint, uint >&                        indexTable,
                                              std::vector< std::unique_ptr< Asset::HandleData > >& data ) const {
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
void AssimpHandleDataLoader::fetchName( const aiMesh& mesh, Asset::HandleData& data, std::set<std::string>& usedNames ) const
{
    std::string name = assimpToCore(mesh.mName);
    while (usedNames.find(name) != usedNames.end())
    {
        name.append("_");
    }

    usedNames.insert(name);
    data.setName(name);
}

/// TYPE
void AssimpHandleDataLoader::fetchType( const aiMesh& mesh, Asset::HandleData& data ) const {
    data.setType( Asset::HandleData::SKELETON );
    // TO DO: is there a way to know the right type of handle?
}

/// VERTEX SIZE
void AssimpHandleDataLoader::fetchVertexSize( Asset::HandleData& data ) const {
    const uint componentSize = data.getComponentDataSize();
    uint vertexSize = 0;
    for( uint i = 0; i < componentSize; ++i ) {
        for( const auto& item : data.getComponent(i).m_weight ) {
            vertexSize = std::max( vertexSize, ( item.first + 1 ) );
        }
    }
    data.setVertexSize( vertexSize );
}
} // namespace IO
} // namespace Ra
