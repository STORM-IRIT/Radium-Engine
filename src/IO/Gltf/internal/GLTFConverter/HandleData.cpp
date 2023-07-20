#include <Core/Asset/FileData.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <IO/Gltf/internal/GLTFConverter/AccessorReader.hpp>
#include <IO/Gltf/internal/GLTFConverter/HandleData.hpp>
#include <IO/Gltf/internal/GLTFConverter/MeshData.hpp>

// #define BE_VERBOSE
#ifdef BE_VERBOSE
#    include <Core/Utils/Log.hpp>
#endif

using namespace fx;

namespace Ra {
namespace IO {
namespace GLTF {

using namespace Ra::Core;
using namespace Ra::Core::Asset;
using namespace Ra::Core::Utils;

Ra::Core::Asset::HandleData*
HandleDataLoader::loadSkeleton( const fx::gltf::Document& gltfScene,
                                const std::vector<SceneNode>& graphNodes,
                                const std::set<int32_t>& visitedNodes,
                                const fx::gltf::Skin& skin,
                                size_t skinIndex,
                                IntToString& nodeNumToComponentName ) {

    // Create skeleton and initialize its frame
    auto skeleton = new HandleData();
    skeleton->setType( HandleData::SKELETON );
    std::string skeletonName = "Skeleton_" + std::to_string( skinIndex );
    skeleton->setName( skeletonName );
    // Find root node and set its transform to the skeleton
    if ( skin.skeleton >= 0 ) {
#ifdef BE_VERBOSE
        LOG( logINFO ) << "GLTF Skeleton : set root node to node " << skin.skeleton << " ("
                       << graphNodes[skin.skeleton].m_nodeName << ")";
#endif
        skeleton->setFrame( graphNodes[skin.skeleton].m_transform );
    }
    else { LOG( logWARNING ) << "No root node defined for skeleton !!! TODO : find it "; }

    // skeleton joint table and nodeNum <-> nodeName joint mappings
    std::map<std::string, uint> skeletonNameTable;
    HandleDataLoader::StringToInt componentNameToNodeNum;

    // fetch the joints
    auto skeletonJoints =
        buildJoints( gltfScene, graphNodes, skin, nodeNumToComponentName, componentNameToNodeNum );

    // fetch the joints' bindMatrices
    auto jointBindMatrix = getBindMatrices( gltfScene, skin );

    // Map the mesh part name with its jointWeight vector;
    // jointWeight[jointIndexInTheSkinJointsVector] contains the vector of
    // (MeshVertexIndex, weight)
    std::map<std::string, std::vector<std::vector<std::pair<uint, Scalar>>>> allJointWeights;
    std::map<std::string, std::vector<Ra::Core::Transform>> allJointBindMatrices;

    // load joint weights and bind matrices per skinned meshes
    size_t nodeNum = 0;
    for ( auto visited : visitedNodes ) {
        auto& graphNode = graphNodes[visited];
        if ( graphNode.m_skinIndex == int32_t( skinIndex ) ) {
            addMeshesWeightsAndBindMatrices( gltfScene,
                                             graphNode,
                                             nodeNum,
                                             jointBindMatrix,
                                             allJointWeights,
                                             allJointBindMatrices );
            ++nodeNum;
        }
#ifdef BE_VERBOSE
        else {
            LOG( logINFO ) << "GLTF Skeleton : visited node " << visited << "("
                           << graphNode.m_nodeName << ") is not skinned.";
        }
#endif
        if ( graphNode.m_meshIndex >= 0 || graphNode.m_cameraIndex >= 0 ) { ++nodeNum; }
    }

    // Finalize the skeleton
    std::set<int32_t> skinnedNodes;

    // fill all the joints data
    for ( int32_t i = 0; i < int32_t( skeletonJoints.size() ); ++i ) {
        // initialize the weighs and bind matrices
        for ( auto it : allJointWeights ) {
            if ( !it.second[i].empty() ) { skeletonJoints[i].m_weights[it.first] = it.second[i]; }
        }
        for ( auto it : allJointBindMatrices ) {
            if ( !it.second.empty() ) { skeletonJoints[i].m_bindMatrices[it.first] = it.second[i]; }
        }
        skeletonNameTable[skeletonJoints[i].m_name] = i;
        skinnedNodes.insert( componentNameToNodeNum[skeletonJoints[i].m_name] );
    }

    // Set the Radium Skeleton properties
    skeleton->setComponents( skeletonJoints );
    skeleton->setNameTable( skeletonNameTable );
    for ( const auto& it : allJointWeights ) {
        skeleton->addBindMesh( it.first );
    }
    // build the Radium Skeleton topology
    HandleDataLoader::buildSkeletonTopology(
        graphNodes, nodeNumToComponentName, componentNameToNodeNum, skinnedNodes, skeleton );

    return skeleton;
}

// --------
AlignedStdVector<HandleComponentData>
HandleDataLoader::buildJoints( const gltf::Document& gltfScene,
                               const std::vector<SceneNode>& graphNodes,
                               const fx::gltf::Skin& skin,
                               IntToString& nodeNumToComponentName,
                               StringToInt& componentNameToNodeNum ) {
#ifdef BE_VERBOSE
    LOG( logINFO ) << "GLTF Skeleton : buildJoints begin: ";
#endif
    AlignedStdVector<HandleComponentData> skeletonJoints( skin.joints.size(),
                                                          HandleComponentData() );
    for ( size_t i = 0; i < skin.joints.size(); ++i ) {
        skeletonJoints[i].m_name =
            HandleDataLoader::getJointName( gltfScene, nodeNumToComponentName, skin.joints[i] );
        skeletonJoints[i].m_frame                        = graphNodes[skin.joints[i]].m_transform;
        componentNameToNodeNum[skeletonJoints[i].m_name] = skin.joints[i];
#ifdef BE_VERBOSE
        LOG( logINFO ) << "\tGLTF Skeleton : add joint: " << i << "(" << skin.joints[i] << ", "
                       << skeletonJoints[i].m_name << ")";
#endif
    }
#ifdef BE_VERBOSE
    LOG( logINFO ) << "GLTF Skeleton : buildJoints done: ";
#endif
    return skeletonJoints;
}

std::vector<Ra::Core::Transform>
HandleDataLoader::getBindMatrices( const fx::gltf::Document& gltfScene,
                                   const fx::gltf::Skin& skin ) {
    std::vector<Transform> jointBindMatrix( skin.joints.size(), Transform::Identity() );
    auto* invBindMatrices =
        reinterpret_cast<float*>( AccessorReader( gltfScene ).read( skin.inverseBindMatrices ) );
    for ( uint i = 0; i < skin.joints.size(); ++i ) {
        Matrix4 mat;
        mat << invBindMatrices[16 * i], invBindMatrices[16 * i + 1], invBindMatrices[16 * i + 2],
            invBindMatrices[16 * i + 3], invBindMatrices[16 * i + 4], invBindMatrices[16 * i + 5],
            invBindMatrices[16 * i + 6], invBindMatrices[16 * i + 7], invBindMatrices[16 * i + 8],
            invBindMatrices[16 * i + 9], invBindMatrices[16 * i + 10], invBindMatrices[16 * i + 11],
            invBindMatrices[16 * i + 12], invBindMatrices[16 * i + 13],
            invBindMatrices[16 * i + 14], invBindMatrices[16 * i + 15];

        jointBindMatrix[i] = Transform( mat.transpose() );
    }
    return jointBindMatrix;
}

std::string HandleDataLoader::getJointName( const gltf::Document& gltfscene,
                                            std::map<int32_t, std::string>& nodeNumToComponentName,
                                            int32_t nodeNum ) {
    auto it = nodeNumToComponentName.find( nodeNum );
    if ( it != nodeNumToComponentName.end() ) { return it->second; }
    else {
        std::string hcName = gltfscene.nodes[nodeNum].name;
        if ( hcName.empty() ) { hcName = "bone_" + std::to_string( nodeNum ); }
        nodeNumToComponentName[nodeNum] = hcName;
        return hcName;
    }
}

void HandleDataLoader::addMeshesWeightsAndBindMatrices(
    const fx::gltf::Document& gltfScene,
    const SceneNode& graphNode,
    int32_t nodeNum,
    std::vector<Ra::Core::Transform>& bindMatrices,
    std::map<std::string, std::vector<std::vector<std::pair<uint, Scalar>>>>& jointsWeights,
    std::map<std::string, std::vector<Ra::Core::Transform>>& jointsMatrices ) {

    VectorArray<Vector4ui> joints {};
    VectorArray<Vector4f> weights {};
    uint numJoints; // Number of joints in the meshPart
    const auto& meshParts = gltfScene.meshes[graphNode.m_meshIndex].primitives;
    for ( uint partNum = 0; partNum < meshParts.size(); partNum++ ) {
        // Ensure the name is the same thant for mesh loading --> make a naming
        // function ....
        std::string meshPartName = gltfScene.meshes[graphNode.m_meshIndex].name;
        if ( meshPartName.empty() ) {
            meshPartName = "mesh_";
            meshPartName += "_n_" + std::to_string( nodeNum ) + "_m_" +
                            std::to_string( graphNode.m_meshIndex ) + "_p_" +
                            std::to_string( partNum );
        }

        joints.clear();
        weights.clear();
        numJoints = 0;
        // Fill joints and weights from accessors
        for ( const auto& attrib : meshParts[partNum].attributes ) {
            if ( attrib.first.substr( 0, 7 ) == "JOINTS_" ) {
                MeshData::GetJoints( joints, gltfScene, gltfScene.accessors[attrib.second] );
                auto weightAccessor = gltfScene.accessors[meshParts[partNum].attributes.at(
                    "WEIGHTS_" + attrib.first.substr( 7 ) )];
                MeshData::GetWeights( weights, gltfScene, weightAccessor );
                numJoints = joints.size();
            }
        }
        // Change the way to access info to conform with FileData
        // jointWeight[jointIndexInTheSkinJointsVector] contains the vector of
        // (MeshVertexIndex, weight)
        std::vector<std::vector<std::pair<uint, Scalar>>> jointWeights(
            bindMatrices.size(), std::vector<std::pair<uint, Scalar>>() );
        float w;
        for ( uint i = 0; i < joints.size(); ++i ) {
            for ( auto iJoint = 0; iJoint < 4; iJoint++ ) {
                w = weights[i]( iJoint, 0 );
                if ( w != 0. ) {
                    jointWeights[joints[i]( iJoint, 0 )].emplace_back(
                        std::pair<uint, Scalar>( i % numJoints, w ) );
                }
            }
        }
        jointsWeights[meshPartName]  = jointWeights;
        jointsMatrices[meshPartName] = bindMatrices;
    }
}

void HandleDataLoader::buildSkeletonTopology( const std::vector<SceneNode>& graphNodes,
                                              const IntToString& nodeNumToComponentName,
                                              const StringToInt& componentNameToNodeNum,
                                              const std::set<int32_t>& skinnedNodes,
                                              HandleData* handle ) {
    std::vector<std::pair<std::string, std::string>> edgeList;
    const auto& skeletonJoints = handle->getComponentData();
    for ( auto& component : skeletonJoints ) {
        const auto& node = graphNodes[componentNameToNodeNum.at( component.m_name )];
        for ( auto j = 0; j < int32_t( node.children.size() ); ++j ) {
            const auto itChild = skinnedNodes.find( node.children[j] );
            if ( itChild != skinnedNodes.end() ) {
                edgeList.emplace_back( std::pair<std::string, std::string> {
                    component.m_name, nodeNumToComponentName.at( node.children[j] ) } );
            } // TODO, if a non skinned subgraph is attached to the node, add an edge for it ?
            else {
#ifdef BE_VERBOSE
                LOG( logINFO ) << "GLTF Skeleton : buildSkeletonTopology found non skinned "
                                  "subgraph between "
                               << component.m_name << " and "
                               << graphNodes[node.children[j]].m_nodeName << "!";

                //                edgeList.emplace_back( std::pair<std::string, std::string> {
                //                    component.m_name, graphNodes[node.children[j]].m_nodeName} );

#endif
            }
        }
    }

    AlignedStdVector<Vector2ui> edge;
    edge.reserve( edgeList.size() );
    std::transform( edgeList.cbegin(),
                    edgeList.cend(),
                    std::back_inserter( edge ),
                    [handle]( auto p ) -> Vector2ui {
                        return { handle->getIndexOf( p.first ), handle->getIndexOf( p.second ) };
                    } );

    handle->setEdges( edge );
}

} // namespace GLTF
} // namespace IO
} // namespace Ra
