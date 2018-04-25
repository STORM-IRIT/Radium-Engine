#ifndef RADIUMENGINE_ASSIMP_ANIMATION_DATA_LOADER_HPP
#define RADIUMENGINE_ASSIMP_ANIMATION_DATA_LOADER_HPP

#include <Core/Asset/DataLoader.hpp>
#include <Core/Asset/AnimationTime.hpp> // Asset::Time
#include <IO/RaIO.hpp>

struct aiScene;
struct aiAnimation;
struct aiNodeAnim;

namespace Ra {
namespace Core {
namespace Asset {
class AnimationData;
struct HandleAnimation;
} // namespace Asset
} // namespace Core
} // namespace Ra

namespace Ra {
namespace IO {

class RA_IO_API AssimpAnimationDataLoader : public Core::Asset::DataLoader<Core::Asset::AnimationData> {
  public:
    /// CONSTRUCTOR
    AssimpAnimationDataLoader( const bool VERBOSE_MODE = false );

    /// DESTRUCTOR
    ~AssimpAnimationDataLoader();

    /// LOADING
    void loadData( const aiScene* scene,
                   std::vector<std::unique_ptr<Core::Asset::AnimationData>>& data ) override;

  protected:
    /// QUERY
    bool sceneHasAnimation( const aiScene* scene ) const;
    uint sceneAnimationSize( const aiScene* scene ) const;

    /// NAME
    void fetchName( const aiAnimation* anim, Core::Asset::AnimationData* data ) const;

    /// TIME
    void fetchTime( const aiAnimation* anim, Core::Asset::AnimationData* data ) const;

    /// KEY FRAME
    void loadAnimationData( const aiScene* scene,
                            std::vector<std::unique_ptr<Core::Asset::AnimationData>>& data ) const;
    void fetchAnimation( const aiAnimation* anim, Core::Asset::AnimationData* data ) const;
    void fetchHandleAnimation( aiNodeAnim* node, Core::Asset::HandleAnimation& data,
                               const Core::Asset::Time dt ) const;
};

} // namespace IO
} // namespace Ra

#endif // RADIUMENGINE_ASSIMP_ANIMATION_DATA_LOADER_HPP
