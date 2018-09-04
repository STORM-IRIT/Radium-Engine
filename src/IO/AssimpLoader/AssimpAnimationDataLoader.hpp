#ifndef RADIUMENGINE_ASSIMP_ANIMATION_DATA_LOADER_HPP
#define RADIUMENGINE_ASSIMP_ANIMATION_DATA_LOADER_HPP

#include <Core/File/DataLoader.hpp>
#include <Core/File/KeyFrame/AnimationTime.hpp> // Asset::Time
#include <IO/RaIO.hpp>

struct aiScene;
struct aiAnimation;
struct aiNodeAnim;

namespace Ra {
namespace Asset {
class AnimationData;
struct HandleAnimation;
} // namespace Asset
} // namespace Ra

namespace Ra {
namespace IO {

/// The AssimpAnimationDataLoader converts animation data from the Assimp format
/// to the Asset::AnimationData format.
class RA_IO_API AssimpAnimationDataLoader : public Asset::DataLoader<Asset::AnimationData> {
  public:
    AssimpAnimationDataLoader( const bool VERBOSE_MODE = false );

    ~AssimpAnimationDataLoader();

    /// Convert all the animation data from \p scene into \p data.
    void loadData( const aiScene* scene,
                   std::vector<std::unique_ptr<Asset::AnimationData>>& data ) override;

  protected:
    /// Return true if the given scene has animation data.
    bool sceneHasAnimation( const aiScene* scene ) const;

    /// Return the number of AssImp animation data in the given scene.
    uint sceneAnimationSize( const aiScene* scene ) const;

    /// Fill \p data with all the AnimationData from \p scene.
    void loadAnimationData( const aiScene* scene,
                            std::vector<std::unique_ptr<Asset::AnimationData>>& data ) const;

    /// Fill \p data with the name from \p anim.
    void fetchName( const aiAnimation* anim, Asset::AnimationData* data ) const;

    /// Fill \p data with the AnimationTime from \p anim.
    void fetchTime( const aiAnimation* anim, Asset::AnimationData* data ) const;

    /// Fill \p data with the AnimationData from \p anim.
    void fetchAnimation( const aiAnimation* anim, Asset::AnimationData* data ) const;

    ///  Fill \p data with the HandleAnimation from \p anim, according to the
    /// animation timestep \p dt.
    void fetchHandleAnimation( aiNodeAnim* node, Asset::HandleAnimation& data,
                               const Asset::Time dt ) const;
};

} // namespace IO
} // namespace Ra

#endif // RADIUMENGINE_ASSIMP_ANIMATION_DATA_LOADER_HPP
