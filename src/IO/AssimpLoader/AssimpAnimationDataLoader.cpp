#include <IO/AssimpLoader/AssimpAnimationDataLoader.hpp>

#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <set>

#include <Core/Animation/KeyRotation.hpp>
#include <Core/Animation/KeyScaling.hpp>
#include <Core/Animation/KeyTranslation.hpp>
#include <Core/File/AnimationData.hpp>
#include <Core/Utils/Log.hpp>

#include <IO/AssimpLoader/AssimpWrapper.hpp>

namespace Ra {
namespace IO {

using namespace Core::Utils; // log
using namespace Core::Animation;

/// CONSTRUCTOR
AssimpAnimationDataLoader::AssimpAnimationDataLoader( const bool VERBOSE_MODE ) :
    DataLoader<Asset::AnimationData>( VERBOSE_MODE ) {}

/// DESTRUCTOR
AssimpAnimationDataLoader::~AssimpAnimationDataLoader() = default;

/// LOADING
void AssimpAnimationDataLoader::loadData(
    const aiScene* scene, std::vector<std::unique_ptr<Asset::AnimationData>>& data ) {
    data.clear();

    if ( scene == nullptr )
    {
        LOG( logDEBUG ) << "AssimpAnimationDataLoader : scene is nullptr.";
        return;
    }

    if ( !sceneHasAnimation( scene ) )
    {
        LOG( logDEBUG ) << "AssimpAnimationDataLoader : scene has no animation.";
        return;
    }

    if ( m_verbose )
    {
        LOG( logDEBUG ) << "File contains animation.";
        LOG( logDEBUG ) << "Animation Loading begin...";
    }

    loadAnimationData( scene, data );

    if ( m_verbose )
    {
        LOG( logDEBUG ) << "Animation Loading end.\n";
    }
}

/// QUERY
bool AssimpAnimationDataLoader::sceneHasAnimation( const aiScene* scene ) const {
    return ( sceneAnimationSize( scene ) != 0 );
}

uint AssimpAnimationDataLoader::sceneAnimationSize( const aiScene* scene ) const {
    if ( scene->HasAnimations() )
    {
        return scene->mNumAnimations;
    }
    return 0;
}

/// NAME
void AssimpAnimationDataLoader::fetchName( const aiAnimation* anim,
                                           Asset::AnimationData* data ) const {
    data->setName( assimpToCore( anim->mName ) );
}

/// TIME
void AssimpAnimationDataLoader::fetchTime( const aiAnimation* anim,
                                           Asset::AnimationData* data ) const {
    const auto tick = Scalar( anim->mTicksPerSecond );
    const auto duration = Scalar( anim->mDuration );

    AnimationTime time;
    Time dt;
    time.setStart( 0.0 );
    if ( tick == Scalar( 0 ) )
    {
        dt = 0.0;
        time.setEnd( duration );
    } else
    {
        dt = Scalar( 1.0 ) / tick;
        time.setEnd( dt * duration );
    }
    data->setTime( time );
    data->setTimeStep( dt );
}

/// KEY FRAME
void AssimpAnimationDataLoader::loadAnimationData(
    const aiScene* scene, std::vector<std::unique_ptr<Asset::AnimationData>>& data ) const {
    const uint size = sceneAnimationSize( scene );
    data.resize( size );
    for ( uint i = 0; i < size; ++i )
    {
        aiAnimation* anim = scene->mAnimations[i];
        Asset::AnimationData* animData = new Asset::AnimationData();
        fetchName( anim, animData );
        fetchTime( anim, animData );
        fetchAnimation( anim, animData );
        if ( m_verbose )
        {
            animData->displayInfo();
        }
        data[i].reset( animData );
    }
}

void AssimpAnimationDataLoader::fetchAnimation( const aiAnimation* anim,
                                                Asset::AnimationData* data ) const {
    const uint size = anim->mNumChannels;
    AnimationTime time = data->getTime();
    std::vector<Asset::HandleAnimation> keyFrame( size );
    for ( uint i = 0; i < size; ++i )
    {
        fetchHandleAnimation( anim->mChannels[i], keyFrame[i], data->getTimeStep() );
        time.merge( keyFrame[i].m_anim.getAnimationTime() );
    }
    data->setFrames( keyFrame );
    data->setTime( time );
}

void AssimpAnimationDataLoader::fetchHandleAnimation( aiNodeAnim* node,
                                                      Asset::HandleAnimation& data,
                                                      const Time dt ) const {
    const uint T_size = node->mNumPositionKeys;
    const uint R_size = node->mNumRotationKeys;
    const uint S_size = node->mNumScalingKeys;

    KeyTranslation tr;
    KeyRotation rot;
    KeyScaling s;

    data.m_anim.reset();
    std::set<Time> keyFrame;

    for ( uint i = 0; i < T_size; ++i )
    {
        auto time = Scalar( node->mPositionKeys[i].mTime );
        aiVector3t<Scalar> value = node->mPositionKeys[i].mValue;
        tr.insertKeyFrame( time, assimpToCore( value ) );
        keyFrame.insert( time );
    }

    for ( uint i = 0; i < R_size; ++i )
    {
        auto time = Scalar( node->mRotationKeys[i].mTime );
        aiQuaternion value = node->mRotationKeys[i].mValue;
        rot.insertKeyFrame( time, assimpToCore( value ) );
        keyFrame.insert( time );
    }

    for ( uint i = 0; i < S_size; ++i )
    {
        auto time = Scalar( node->mScalingKeys[i].mTime );
        aiVector3t<Scalar> value = node->mScalingKeys[i].mValue;
        s.insertKeyFrame( time, assimpToCore( value ) );
        keyFrame.insert( time );
    }

    data.m_name = assimpToCore( node->mNodeName );
    data.m_anim.setAnimationTime( AnimationTime( *keyFrame.begin(), *keyFrame.rbegin() ) );
    for ( const auto& time : keyFrame )
    {
        Core::Transform T;
        T.fromPositionOrientationScale( tr.at( time ), rot.at( time ), s.at( time ) );
        data.m_anim.insertKeyFrame( ( ( dt == 0 ) ? time : ( dt * time ) ), T );
    }
}

} // namespace IO
} // namespace Ra
