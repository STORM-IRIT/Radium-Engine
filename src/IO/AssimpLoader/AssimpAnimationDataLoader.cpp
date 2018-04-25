#include <IO/AssimpLoader/AssimpAnimationDataLoader.hpp>

#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <set>

#include <Core/Asset/KeyTranslation.hpp>
#include <Core/Asset/KeyScaling.hpp>
#include <Core/Asset/KeyRotation.hpp>
#include <Core/Asset/AnimationData.hpp>
#include <Core/Utils/Log.hpp>

#include <IO/AssimpLoader/AssimpWrapper.hpp>

namespace Ra {
namespace IO {

/// CONSTRUCTOR
AssimpAnimationDataLoader::AssimpAnimationDataLoader( const bool VERBOSE_MODE ) :
    DataLoader<Core::Asset::AnimationData>( VERBOSE_MODE ) {}

/// DESTRUCTOR
AssimpAnimationDataLoader::~AssimpAnimationDataLoader() {}

/// LOADING
void AssimpAnimationDataLoader::loadData(
    const aiScene* scene, std::vector<std::unique_ptr<Core::Asset::AnimationData>>& data ) {
    data.clear();

    if ( scene == nullptr )
    {
        LOG( Core::Utils::logDEBUG ) << "AssimpAnimationDataLoader : scene is nullptr.";
        return;
    }

    if ( !sceneHasAnimation( scene ) )
    {
        LOG( Core::Utils::logDEBUG ) << "AssimpAnimationDataLoader : scene has no animation.";
        return;
    }

    if ( m_verbose )
    {
        LOG( Core::Utils::logDEBUG ) << "File contains animation.";
        LOG( Core::Utils::logDEBUG ) << "Animation Loading begin...";
    }

    loadAnimationData( scene, data );

    if ( m_verbose )
    {
        LOG( Core::Utils::logDEBUG ) << "Animation Loading end.\n";
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
                                           Core::Asset::AnimationData* data ) const {
    data->setName( assimpToCore( anim->mName ) );
}

/// TIME
void AssimpAnimationDataLoader::fetchTime( const aiAnimation* anim,
                                           Core::Asset::AnimationData* data ) const {
    const Scalar tick = anim->mTicksPerSecond;
    const Scalar duration = anim->mDuration;

    Core::Asset::AnimationTime time;
    Core::Asset::Time dt;
    time.setStart( 0.0 );
    if ( tick == 0 )
    {
        dt = 0.0;
        time.setEnd( duration );
    } else
    {
        dt = 1.0 / tick;
        time.setEnd( dt * duration );
    }
    data->setTime( time );
    data->setTimeStep( dt );
}

/// KEY FRAME
void AssimpAnimationDataLoader::loadAnimationData(
    const aiScene* scene, std::vector<std::unique_ptr<Core::Asset::AnimationData>>& data ) const {
    const uint size = sceneAnimationSize( scene );
    data.resize( size );
    for ( uint i = 0; i < size; ++i )
    {
        aiAnimation* anim = scene->mAnimations[i];
        Core::Asset::AnimationData* animData = new Core::Asset::AnimationData();
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
                                                Core::Asset::AnimationData* data ) const {
    const uint size = anim->mNumChannels;
    Core::Asset::AnimationTime time = data->getTime();
    std::vector<Core::Asset::HandleAnimation> keyFrame( size );
    for ( uint i = 0; i < size; ++i )
    {
        fetchHandleAnimation( anim->mChannels[i], keyFrame[i], data->getTimeStep() );
        time.merge( keyFrame[i].m_anim.getAnimationTime() );
    }
    data->setFrames( keyFrame );
    data->setTime( time );
}

void AssimpAnimationDataLoader::fetchHandleAnimation( aiNodeAnim* node,
                                                      Core::Asset::HandleAnimation& data,
                                                      const Core::Asset::Time dt ) const {
    const uint T_size = node->mNumPositionKeys;
    const uint R_size = node->mNumRotationKeys;
    const uint S_size = node->mNumScalingKeys;

    Core::Asset::KeyTranslation tr;
    Core::Asset::KeyRotation rot;
    Core::Asset::KeyScaling s;

    data.m_anim.reset();
    std::set<Core::Asset::Time> keyFrame;

    for ( uint i = 0; i < T_size; ++i )
    {
        Scalar time = node->mPositionKeys[i].mTime;
        aiVector3t<Scalar> value = node->mPositionKeys[i].mValue;
        tr.insertKeyFrame( time, assimpToCore( value ) );
        keyFrame.insert( time );
    }

    for ( uint i = 0; i < R_size; ++i )
    {
        Scalar time = node->mRotationKeys[i].mTime;
        aiQuaternion value = node->mRotationKeys[i].mValue;
        rot.insertKeyFrame( time, assimpToCore( value ) );
        keyFrame.insert( time );
    }

    for ( uint i = 0; i < S_size; ++i )
    {
        Scalar time = node->mScalingKeys[i].mTime;
        aiVector3t<Scalar> value = node->mScalingKeys[i].mValue;
        s.insertKeyFrame( time, assimpToCore( value ) );
        keyFrame.insert( time );
    }

    data.m_name = assimpToCore( node->mNodeName );
    data.m_anim.setAnimationTime( Core::Asset::AnimationTime( *keyFrame.begin(), *keyFrame.rbegin() ) );
    for ( const auto& time : keyFrame )
    {
        Core::Math::Transform T;
        T.fromPositionOrientationScale( tr.at( time ), rot.at( time ), s.at( time ) );
        data.m_anim.insertKeyFrame( ( ( dt == 0 ) ? time : ( dt * time ) ), T );
    }
}

} // namespace IO
} // namespace Ra
