#include <IO/AssimpLoader/AssimpHandleDataLoader.hpp>

#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <queue>
#include <set>

#include <Core/Asset/HandleData.hpp>
#include <Core/Containers/AlignedStdVector.hpp>
#include <Core/Utils/Log.hpp>

#include <IO/AssimpLoader/AssimpWrapper.hpp>

namespace Ra {
namespace IO {

using namespace Core::Utils; // log
using namespace Core::Asset;

/// CONSTRUCTOR
AssimpHandleDataLoader::AssimpHandleDataLoader( const bool VERBOSE_MODE ) :
    DataLoader<HandleData>( VERBOSE_MODE ){};

/// DESTRUCTOR
AssimpHandleDataLoader::~AssimpHandleDataLoader() = default;

/// LOAD
void AssimpHandleDataLoader::loadData( const aiScene* scene,
                                       std::vector<std::unique_ptr<HandleData>>& data ) {
    data.clear();

    if ( scene == nullptr )
    {
        LOG( logDEBUG ) << "AssimpHandleDataLoader : scene is nullptr.";
        return;
    }

    if ( !sceneHasHandle( scene ) )
    {
        LOG( logDEBUG ) << "AssimpHandleDataLoader : scene has no handle.";
        return;
    }

    if ( m_verbose )
    {
        LOG( logDEBUG ) << "File contains handle.";
        LOG( logDEBUG ) << "Handle Loading begin...";
    }

    loadHandleData( scene, data );

    if ( m_verbose )
    {
        LOG( logDEBUG ) << "Handle Loading end.\n";
    }
}

/// QUERY
bool AssimpHandleDataLoader::sceneHasHandle( const aiScene* scene ) const {
    return ( sceneHandleSize( scene ) != 0 );
}

uint AssimpHandleDataLoader::sceneHandleSize( const aiScene* scene ) const {
    uint handle_size = 0;
    const uint size = scene->mNumMeshes;
    for ( uint i = 0; i < size; ++i )
    {
        aiMesh* mesh = scene->mMeshes[i];
        if ( mesh->HasBones() )
        {
            ++handle_size;
        }
    }
    return handle_size;
}

namespace {

aiNode* findMeshNode( aiNode* node, const aiScene* scene, const aiString& meshName ) {
    // look inside the node
    for ( uint i = 0; i < node->mNumMeshes; ++i )
    {
        if ( scene->mMeshes[node->mMeshes[i]]->mName == meshName )
        {
            return node;
        }
    }
    // repeat on children
    for ( uint i = 0; i < node->mNumChildren; ++i )
    {
        aiNode* n = findMeshNode( node->mChildren[i], scene, meshName );
        if ( n != nullptr )
        {
            return n;
        }
    }
    return nullptr;
}

void initMarks( const aiNode* node, std::map<std::string, bool>& flag ) {
    flag[assimpToCore( node->mName )] = false;
    for ( uint i = 0; i < node->mNumChildren; ++i )
    {
        initMarks( node->mChildren[i], flag );
    }
}

void markParents( aiNode* node, const aiScene* scene, const std::vector<aiNode*>& meshParents,
                  std::map<std::string, bool>& flag,
                  std::map<std::string, aiNode*>& skelRootToMeshNode ) {
    const std::string nodeName = assimpToCore( node->mName );
    flag[nodeName] = true;
    // check if node is in the mesh hierarchy
    auto it = std::find_if( meshParents.begin(), meshParents.end(),
                            [node]( const auto& n ) { return n == node; } );
    if ( it != meshParents.end() )
    {
        flag[nodeName] = false;
        return;
    }
    // otherwise mark parents
    if ( node->mParent != nullptr )
    {
        markParents( node->mParent, scene, meshParents, flag, skelRootToMeshNode );
        if ( !flag[assimpToCore( node->mParent->mName )] )
        {
            // parent is in the mesh hierarchy, associate node and meshNode
            skelRootToMeshNode[nodeName] = meshParents[0];
        }
    }
}

aiNode* findBoneChild( aiNode* node,
                       const std::map<std::string, Core::Transform>& meshBoneOffset ) {
    for ( uint i = 0; i < node->mNumChildren; ++i )
    {
        if ( meshBoneOffset.find( assimpToCore( node->mChildren[i]->mName ) ) !=
             meshBoneOffset.end() )
        {
            return node->mChildren[i];
        }
    }
    // not found, go down
    for ( uint i = 0; i < node->mNumChildren; ++i )
    {
        auto child = findBoneChild( node->mChildren[i], meshBoneOffset );
        if ( child != nullptr )
        {
            return child;
        }
    }
    // not on this branch, parent will check siblings
    return nullptr;
}

} // namespace

/// LOAD
void AssimpHandleDataLoader::loadHandleData(
    const aiScene* scene, std::vector<std::unique_ptr<HandleData>>& data ) const {
    // list mesh names according to GeometryLoader naming
    std::set<std::string> meshNames;
    // map from bone name to aiBone for offset matrices
    std::map<std::string, Core::Transform> meshBoneOffset;

    // initialize need flag on all scene nodes
    std::map<std::string, bool> needNode;
    initMarks( scene->mRootNode, needNode );
    std::map<std::string, aiNode*> skelRootToMeshNode;

    // load the HandleComponentData for all meshes
    std::map<std::string, HandleComponentData> mapBone2Data;
    for ( uint n = 0; n < scene->mNumMeshes; ++n )
    {
        const aiMesh* mesh = scene->mMeshes[n];
        // fetch mesh name as registered by the GeometryLoader
        std::string meshName = assimpToCore( mesh->mName );
        while ( meshNames.find( meshName ) != meshNames.end() )
        {
            meshName.append( "_" );
        }
        meshNames.insert( meshName );

        // if bound to no skeleton, skip
        if ( !mesh->HasBones() )
        {
            continue;
        }

        // get mesh node parents
        std::vector<aiNode*> meshParents;
        aiNode* meshNode = findMeshNode( scene->mRootNode, scene, mesh->mName );
        while ( meshNode != nullptr )
        {
            meshParents.push_back( meshNode );
            meshNode = meshNode->mParent;
        }

        // deal with skeleton
        for ( uint j = 0; j < mesh->mNumBones; ++j )
        {
            const aiBone* bone = mesh->mBones[j];
            // fetch bone data
            const std::string boneName = assimpToCore( bone->mName );
            meshBoneOffset[boneName] = assimpToCore( bone->mOffsetMatrix );
            // if data doesn't exist yet, create it
            mapBone2Data[boneName].m_name = boneName;
            // fill skinning weights for this mesh
            loadHandleComponentDataWeights( bone, meshName, mapBone2Data[boneName] );
            // deal with hierarchy
            aiNode* node = scene->mRootNode->FindNode( bone->mName );
            if ( node == nullptr )
            {
                continue;
            }
            // mark parents as needed up to mesh node relative
            markParents( node, scene, meshParents, needNode, skelRootToMeshNode );
            // check children since end bones may not have weights
            if ( node->mNumChildren == 1 )
            {
                const aiNode* child = node->mChildren[0];
                const std::string childName = assimpToCore( child->mName );
                // mark as needed
                needNode[childName] = true;
                // if data doesn't exist yet, create it
                mapBone2Data[childName].m_name = childName;
            }
        }
    }

    // load hierarchy for needed bones
    std::vector<std::pair<std::string, std::string>> edgeList;
    for ( const auto& n : needNode )
    {
        if ( !n.second )
        {
            continue;
        }
        // if data doesn't exist, create it (bone with no weight)
        mapBone2Data[n.first].m_name = n.first;
        // link to children
        const aiNode* node = scene->mRootNode->FindNode( aiString( n.first ) );
        for ( uint j = 0; j < node->mNumChildren; ++j )
        {
            const std::string childName = assimpToCore( node->mChildren[j]->mName );
            if ( needNode.at( childName ) )
            {
                // if data doesn't exist, create it (bone with no weight)
                mapBone2Data[childName].m_name = childName;
                // register parenthood
                edgeList.push_back( {n.first, childName} );
            }
        }
    }

    // load bone frame once all are registered (also deal with offset)
    bool offsetOk = true;
    for ( auto& bone : mapBone2Data )
    {
        loadHandleComponentDataFrame( scene, aiString( bone.first ), bone.second );
        auto it = meshBoneOffset.find( bone.first );
        if ( it != meshBoneOffset.end() )
        {
            mapBone2Data[it->first].m_offset = it->second;
        } else
        {
            // FIXME: Assimp does not provide the offet matrix for non-bound bones
            //        in the hierarchy. Hence there is no way to get the full
            //        bind pose skeleton in such a case.
            //        The code below (2 versions) try to infer the offset matrix
            //        for such bones through the local transformation of the
            //        bones' ascendant/descendant with an offset matrix.
            // WARNING: None of these versions is correct, since the computation
            //          of the offset matrix is some sort of black magic that
            //          does not exactly use the local transforms of nodes.
            //          Hence, one version may work for some models but not
            //          others, for which the second version is preferred.
            //          There could also be models for which none of the
            //          versions work.
            // NOTE: Model examples include:
            //       - spiderman.fbx: the first version works, the second version
            //                        doesn't load bones correctly, leading to
            //                        strange movements during animations.
            //       - dragon.fbx: the first version works, the second version
            //                     doesn't load wing-attachement bones correctly
            //                     (the animation seams correct though).
            //       - astroboy.dae: the second version works, the first version
            //                       doesn't load the hands' palm bones correctly
            //                       (the animation seams correct though).
            //       - humanoid.fbx: the second version works, the first version
            //                       doesn't load bones correctly, leading to
            //                       strange movements during animations.

            auto node = scene->mRootNode->FindNode( aiString( bone.first ) );

            // check children and parents for a bone
            auto child = findBoneChild( node, meshBoneOffset );
            std::queue<Core::Transform> parents;
            parents.push( assimpToCore( node->mTransformation ) );
            auto parent = node;
            while ( parent->mParent != nullptr )
            {
                parent = parent->mParent;
                it = meshBoneOffset.find( assimpToCore( parent->mName ) );
                if ( it != meshBoneOffset.end() )
                {
                    break;
                }
                parents.push( assimpToCore( parent->mTransformation ) );
            }
            bool ok = ( it == meshBoneOffset.end() || child == nullptr );
            offsetOk &= ok;

            Core::Transform offset = Core::Transform::Identity();
#if 1 // First version: try computing from children first, then from parents
      // go down up to first bone child, if any
            if ( child != nullptr )
            {
                offset = meshBoneOffset[assimpToCore( child->mName )];
                while ( child != node )
                {
                    offset = assimpToCore( child->mTransformation ) * offset;
                    child = child->mParent;
                }
                mapBone2Data[bone.first].m_offset = offset;
            } else // no child with offset found, go for a parent
            {
                offset = it->second;
                // go down to the node
                while ( !parents.empty() )
                {
                    offset = parents.front().inverse() * offset;
                    parents.pop();
                }
                mapBone2Data[bone.first].m_offset = offset;
            }
#else // Second version: try computing from parents first, then from children
            if ( it == meshBoneOffset.end() )
            {
                // went up to root node, go down up to first bone child
                offset = meshBoneOffset[assimpToCore( child->mName )];
                while ( child != node )
                {
                    offset = assimpToCore( child->mTransformation ) * offset;
                    child = child->mParent;
                }
                mapBone2Data[bone.first].m_offset = offset;
            } else
            {
                offset = it->second;
                // go down to the node
                while ( !parents.empty() )
                {
                    offset = parents.front().inverse() * offset;
                    parents.pop();
                }
                mapBone2Data[bone.first].m_offset = offset;
            }
#endif
        }
    }
    if ( !offsetOk )
    {
        LOG( logWARNING ) << "some bones don't have influence, skinning may misbehave.";
    }

    // find roots and leaves
    std::set<std::string> roots;
    for ( const auto& node : needNode )
    {
        if ( node.second )
        {
            roots.insert( node.first );
        }
    }
    std::set<std::string> leaves = roots;
    for ( auto edge : edgeList )
    {
        roots.erase( edge.second );
        leaves.erase( edge.first );
    }

    // build one HandleData per root
    for ( auto root : roots )
    {
        HandleData* handle = new HandleData();
        handle->setType( HandleData::SKELETON );
        handle->setName( root );

        // Fetch the skeleton frame
        Core::Transform frame = Core::Transform::Identity();
        aiNode* node = skelRootToMeshNode[root];
        while ( node != nullptr )
        {
            frame = assimpToCore( node->mTransformation ) * frame;
            node = node->mParent;
        }
        handle->setFrame( frame );

        // get list of bones and edges for this skeleton
        std::map<std::string, uint> nameTable;
        fillHandleData( root, edgeList, mapBone2Data, nameTable, handle );
        handle->setNameTable( nameTable );

        // check if need additional end bones
        bool needEndBone = false;
        for ( const std::string& leaf : leaves )
        {
            if ( nameTable.find( leaf ) != nameTable.end() )
            {
                const auto& handleComponentData = mapBone2Data[leaf];
                for ( const auto& mesh : handleComponentData.m_weight )
                {
                    if ( mesh.second.size() != 0 )
                    {
                        needEndBone = true;
                        break;
                    }
                }
                if ( needEndBone )
                {
                    break;
                }
            }
        }
        handle->needEndNodes( needEndBone );

        // register the HandleData
        data.emplace_back( handle );
        if ( m_verbose )
        {
            handle->displayInfo();
        }
    }
}

void AssimpHandleDataLoader::loadHandleComponentDataFrame( const aiScene* scene,
                                                           const aiString& boneName,
                                                           HandleComponentData& data ) const {
    // fetch global transform
    data.m_frame.setIdentity();
    aiNode* node = scene->mRootNode->FindNode( boneName );
    while ( node != nullptr )
    {
        data.m_frame = assimpToCore( node->mTransformation ) * data.m_frame;
        node = node->mParent;
    }
}

void AssimpHandleDataLoader::loadHandleComponentDataWeights( const aiBone* bone,
                                                             const std::string& meshName,
                                                             HandleComponentData& data ) const {
    // fetch skinning weigthts
    const uint size = bone->mNumWeights;
    for ( uint j = 0; j < size; ++j )
    {
        std::pair<uint, Scalar> weight( bone->mWeights[j].mVertexId, bone->mWeights[j].mWeight );
        data.m_weight[meshName].push_back( weight );
    }
}

void AssimpHandleDataLoader::fillHandleData(
    const std::string& node, const std::vector<std::pair<std::string, std::string>>& edgeList,
    const std::map<std::string, HandleComponentData>& mapBone2Data,
    std::map<std::string, uint>& nameTable, HandleData* data ) const {
    // register the HandleComponentData for the bone
    nameTable[node] = data->getComponentData().size();
    data->getComponentData().push_back( mapBone2Data.at( node ) );
    // bind meshes bound to the bone
    for ( const auto& mesh : mapBone2Data.at( node ).m_weight )
    {
        data->addBindMesh( mesh.first );
    }
    // go through children
    for ( const auto& edge : edgeList )
    {
        if ( edge.first == node )
        {
            fillHandleData( edge.second, edgeList, mapBone2Data, nameTable, data );
            data->getEdgeData().push_back(
                {nameTable.at( edge.first ), nameTable.at( edge.second )} );
        }
    }
}

} // namespace IO
} // namespace Ra
