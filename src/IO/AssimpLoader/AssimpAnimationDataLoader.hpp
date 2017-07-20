#ifndef RADIUMENGINE_ASSIMP_ANIMATION_DATA_LOADER_HPP
#define RADIUMENGINE_ASSIMP_ANIMATION_DATA_LOADER_HPP

#include <IO/RaIO.hpp>
#include <Core/File/DataLoader.hpp>
#include <Core/File/KeyFrame/AnimationTime.hpp>

struct aiScene;
struct aiAnimation;
struct aiNodeAnim;

namespace Ra {
namespace Asset {
class AnimationData;
struct HandleAnimation;
}
}

namespace Ra {
namespace IO {

class RA_IO_API AssimpAnimationDataLoader : public Asset::DataLoader< Asset::AnimationData > {
public:
    /// CONSTRUCTOR
    AssimpAnimationDataLoader( const bool VERBOSE_MODE = false );

    /// DESTRUCTOR
    ~AssimpAnimationDataLoader();

    /// LOADING
    void loadData( const aiScene* scene, std::vector< std::unique_ptr< Asset::AnimationData > >& data ) override;

protected:
    /// QUERY
    bool sceneHasAnimation( const aiScene* scene ) const;
    uint sceneAnimationSize( const aiScene* scene ) const;

    /// NAME
    void fetchName( const aiAnimation* anim, Asset::AnimationData* data ) const;

    /// TIME
    void fetchTime( const aiAnimation* anim, Asset::AnimationData* data ) const;

    /// KEY FRAME
    void loadAnimationData( const aiScene* scene, std::vector< std::unique_ptr< Asset::AnimationData > >& data ) const;
    void fetchAnimation( const aiAnimation* anim, Asset::AnimationData* data ) const;
    void fetchHandleAnimation( aiNodeAnim* node, Asset::HandleAnimation& data, const Asset::Time dt ) const;
};

} // namespace IO
} // namespace Ra

#endif // RADIUMENGINE_ASSIMP_ANIMATION_DATA_LOADER_HPP
