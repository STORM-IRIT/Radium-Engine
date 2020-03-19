#ifndef RADIUMENGINE_ASSIMP_ANIMATION_DATA_LOADER_HPP
#define RADIUMENGINE_ASSIMP_ANIMATION_DATA_LOADER_HPP

#include <Core/Asset/AnimationTime.hpp>
#include <Core/Asset/DataLoader.hpp>
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

namespace IO {

/**
 * The AssimpAnimationDataLoader converts animation data from the Assimp format
 * to the Asset::AnimationData format.
 */
class RA_IO_API AssimpAnimationDataLoader
    : public Core::Asset::DataLoader<Core::Asset::AnimationData>
{
  public:
    explicit AssimpAnimationDataLoader( const bool VERBOSE_MODE = false );

    ~AssimpAnimationDataLoader() override;

    void loadData( const aiScene* scene,
                   std::vector<std::unique_ptr<Core::Asset::AnimationData>>& data ) override;

  protected:
    /**
     * \returns true if the given scene has animation data, false otherwise.
     */
    bool sceneHasAnimation( const aiScene* scene ) const;

    /**
     * \returns the number of AssImp animation data in the given scene.
     */
    uint sceneAnimationSize( const aiScene* scene ) const;

    /**
     * Fills \p data with the name from \p anim.
     */
    void fetchName( const aiAnimation* anim, Core::Asset::AnimationData* data ) const;

    /**
     * Fills \p data with the AnimationTime from \p anim.
     */
    void fetchTime( const aiAnimation* anim, Core::Asset::AnimationData* data ) const;

    /**
     * Fills \p data with all the AnimationData from \p scene.
     */
    void loadAnimationData( const aiScene* scene,
                            std::vector<std::unique_ptr<Core::Asset::AnimationData>>& data ) const;

    /**
     * Fills \p data with the AnimationData from \p anim.
     */
    void fetchAnimation( const aiAnimation* anim, Core::Asset::AnimationData* data ) const;

    /**
     * Fills \p data with the HandleAnimation from \p anim, according to the
     * animation timestep \p dt.
     */
    void fetchHandleAnimation( aiNodeAnim* node,
                               Core::Asset::HandleAnimation& data,
                               const Core::Asset::AnimationTime::Time dt ) const;
};

} // namespace IO
} // namespace Ra

#endif // RADIUMENGINE_ASSIMP_ANIMATION_DATA_LOADER_HPP
