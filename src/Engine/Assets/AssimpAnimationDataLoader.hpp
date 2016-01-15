#ifndef RADIUMENGINE_ASSIMP_ANIMATION_DATA_LOADER_HPP
#define RADIUMENGINE_ASSIMP_ANIMATION_DATA_LOADER_HPP

#include <assimp/mesh.h>

#include <Engine/Assets/KeyFrame/AnimationTime.hpp>
#include <Engine/Assets/DataLoader.hpp>

namespace Ra {
namespace Asset {

class AnimationData;
struct HandleAnimation;

class AssimpAnimationDataLoader : public DataLoader< AnimationData > {
public:
    /// CONSTRUCTOR
    AssimpAnimationDataLoader( const bool VERBOSE_MODE = false );

    /// DESTRUCTOR
    ~AssimpAnimationDataLoader();

    /// LOADING
    void loadData( const aiScene* scene, std::vector< std::unique_ptr< AnimationData > >& data ) override;

protected:
    /// QUERY
    bool sceneHasAnimation( const aiScene* scene ) const;
    uint sceneAnimationSize( const aiScene* scene ) const;

    /// NAME
    void fetchName( const aiAnimation* anim, AnimationData* data ) const;

    /// TIME
    void fetchTime( const aiAnimation* anim, AnimationData* data ) const;

    /// KEY FRAME
    void loadAnimationData( const aiScene* scene, std::vector< std::unique_ptr< AnimationData > >& data ) const;
    void fetchAnimation( const aiAnimation* anim, AnimationData* data ) const;
    void fetchHandleAnimation( aiNodeAnim* node, HandleAnimation& data, const Time dt ) const;
};

} // namespace Asset
} // namespace Ra

#endif // RADIUMENGINE_ASSIMP_ANIMATION_DATA_LOADER_HPP
