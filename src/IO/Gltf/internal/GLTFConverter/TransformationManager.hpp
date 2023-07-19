#pragma once
#include <Core/Asset/AnimationData.hpp>
#include <Core/Types.hpp>

#include <IO/Gltf/internal/fx/gltf.h>

#include <map>
#include <set>
#include <string>
#include <vector>

namespace Ra {
namespace IO {
namespace GLTF {

class TransformationManager
{
  public:
    explicit TransformationManager( std::map<int32_t, std::string>& map ) :
        m_nodeIdToBoneName { map } {};
    void insert( int32_t node,
                 const std::string& path,
                 float* times,
                 float* transformations,
                 int32_t count,
                 fx::gltf::Animation::Sampler::Type interpolation,
                 const std::array<float, 4>& nodeRotation,
                 const std::array<float, 3>& nodeScale,
                 const std::array<float, 3>& nodeTranslation );

    void buildAnimation( std::vector<Ra::Core::Asset::HandleAnimation>& animations );

  private:
    std::vector<int32_t> m_nodeVisited;
    std::map<int32_t, std::map<float, Ra::Core::Quaternionf>> m_rotation;
    std::map<int32_t, std::map<float, Ra::Core::Vector3>> m_scale;
    std::map<int32_t, std::map<float, Ra::Core::Vector3>> m_translation;
    std::map<int32_t, std::set<float>> m_times;
    std::map<int32_t, std::string>& m_nodeIdToBoneName;
    std::map<int32_t, std::tuple<std::array<float, 4>, std::array<float, 3>, std::array<float, 3>>>
        m_nodeBaseTransform;
};

} // namespace GLTF
} // namespace IO
} // namespace Ra
