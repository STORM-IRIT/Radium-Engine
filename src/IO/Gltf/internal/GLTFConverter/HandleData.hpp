#pragma once
#include <Core/Asset/HandleData.hpp>
#include <Core/Containers/AlignedStdVector.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <IO/Gltf/internal/GLTFConverter/SceneNode.hpp>

#include <IO/Gltf/internal/fx/gltf.h>

#include <set>
#include <string>

namespace Ra::Core::Asset {
class FileData;
}

namespace Ra {
namespace IO {
namespace GLTF {

/**
 * Functions to load handle data from a json GLTF file
 *
 */
class HandleDataLoader
{
  public:
    using IntToString = std::map<int32_t, std::string>;
    using StringToInt = std::map<std::string, int32_t>;

    /**
     * Load and build the skeleton and skinning weights for a given skin.
     * @param gltfscene The json representation of the gltf scene
     * @param graphNodes The array of active nodes
     * @param visitedNodes The set of node index for active nodes
     * @param skin The gltf skeleton definition
     * @param nodeNumToComponentName the mapping between node numbers and component names
     * @param skeleton_num The number of the loaded skeleton
     * @return The Radium representation of the skeleton with nodes weight matrices
     * @todo verify the bindmatrices of nodes and make attached subgraph to follow the skeleton
     * animation
     */
    static Ra::Core::Asset::HandleData* loadSkeleton( const fx::gltf::Document& gltfScene,
                                                      const std::vector<SceneNode>& graphNodes,
                                                      const std::set<int32_t>& visitedNodes,
                                                      const fx::gltf::Skin& skin,
                                                      size_t skinIndex,
                                                      IntToString& nodeNumToComponentName );

  private:
    static Ra::Core::AlignedStdVector<Ra::Core::Asset::HandleComponentData>
    buildJoints( const fx::gltf::Document& gltfScene,
                 const std::vector<SceneNode>& graphNodes,
                 const fx::gltf::Skin& skin,
                 IntToString& nodeNumToComponentName,
                 StringToInt& componentNameToNodeNum );

    static std::vector<Ra::Core::Transform> getBindMatrices( const fx::gltf::Document& gltfScene,
                                                             const fx::gltf::Skin& skin );

    static std::string getJointName( const fx::gltf::Document& gltfScene,
                                     IntToString& nodeNumToComponentName,
                                     int32_t nodeNum );

    static void addMeshesWeightsAndBindMatrices(
        const fx::gltf::Document& gltfScene,
        const SceneNode& graphNode,
        int32_t nodeNum,
        std::vector<Ra::Core::Transform>& bindMatrices,
        std::map<std::string, std::vector<std::vector<std::pair<uint, Scalar>>>>& jointsWeights,
        std::map<std::string, std::vector<Ra::Core::Transform>>& jointsMatrices );

    static void buildSkeletonTopology( const std::vector<SceneNode>& graphNodes,
                                       const IntToString& nodeNumToComponentName,
                                       const StringToInt& componentNameToNodeNum,
                                       const std::set<int32_t>& insertedNodes,
                                       Ra::Core::Asset::HandleData* handle );
};

} // namespace GLTF
} // namespace IO
} // namespace Ra
