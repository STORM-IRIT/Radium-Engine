#ifndef RADIUMENGINE_ASSIMP_ANIMATION_DATA_LOADER_HPP
#define RADIUMENGINE_ASSIMP_ANIMATION_DATA_LOADER_HPP

#include <assimp/mesh.h>

#include <Core/Log/Log.hpp>

#include <Engine/Assets/AssimpWrapper.hpp>
#include <Engine/Assets/DataLoader.hpp>
#include <Engine/Assets/AnimationData.hpp>
#include <Engine/Assets/FileData.hpp>

// FIXME(Charly): Needs to be fixed to be compiled
#ifdef DEBUG_LOAD_ANIMATION

namespace Ra {
namespace Asset {

class AssimpAnimationDataLoader : public DataLoader< AnimationData >
{
public:
    /// CONSTRUCTOR
    AssimpAnimationDataLoader( const bool VERBOSE_MODE = false ) : DataLoader< AnimationData >( VERBOSE_MODE ) { }

    /// DESTRUCTOR
    ~AssimpAnimationDataLoader() { }

    /// LOADING
    inline void loadData( const aiScene* scene, std::vector< std::unique_ptr< AnimationData > >& data ) const override {
        data.clear();

        if( scene == nullptr ) {
            LOG( logDEBUG ) << "AssimpAnimationDataLoader : scene is nullptr.";
            return;
        }

        if( !sceneHasAnimation( scene ) ) {
            LOG( logDEBUG ) << "AssimpAnimationDataLoader : scene has no animation.";
            return;
        }

        if( m_verbose ) {
            LOG( logDEBUG ) << "File contains animation.";
            LOG( logDEBUG ) << "Animation Loading begin...";
        }

        loadAnimationData( scene, data );

        if( m_verbose ) {
            LOG( logDEBUG ) << "Animation Loading end.\n";
        }
    }



protected:
    /// QUERY
    inline bool sceneHasAnimation( const aiScene* scene ) {
        return ( sceneAnimationSize( scene ) != 0 );
    }



    uint sceneAnimationSize( const aiScene* scene ) {
        if( scene->HasAnimations() ) {
            return scene->mNumAnimations;
        }
        return 0;
    }



    /// NAME
    inline void fetchName( const aiAnimation* anim, AnimationData* data ) {
        data->setName( assimpToCore( anim->mName ) );
    }



    /// TIME
    inline void fetchTime( const aiAnimation* anim, AnimationData* data ) {
        const Scalar tick     = anim->mTicksPerSecond;
        const Scalar duration = anim->mDuration;

        AnimationTime time;
        Time dt;
        time.setStart( 0.0 );
        if( tick == 0 ) {
            dt = 0.0;
            time.setEnd( duration );
        } else {
            dt = 1000.0 / tick;
            time.setEnd( tick * duration );
        }

        data->setTime( time );
        data->setTimeStep( dt );
    }



    /// KEY FRAME
    inline void loadAnimationData( const aiScene* scene, std::vector< std::unique_ptr< AnimationData > >& data ) {
        const uint size = sceneAnimationSize( scene );
        data.resize( size );
        for( uint i = 0; i < size; ++i ) {
            aiAnimation* anim = scene->mAnimations[i];
            data[i].reset( new AnimationData() );
            fetchName( anim, data[i].get() );
            fetchTime( anim, data[i].get() );
            fetchAnimation( anim, data[i].get() );
            if( m_verbose ) {
                data[i]->displayInfo();
            }
        }
    }

    inline void fetchAnimation( const aiAnimation* anim, AnimationData* data ) {
        const uint size = anim->mNumChannels;
        Time time = data->getTime();
        std::vector< HandleAnimation > keyFrame( size );
        for( uint i = 0; i < size; ++i ) {
            fetchHandleAnimation( anim->mChannels[i], keyFrame[i] );
            time.merge( keyFrame[i].m_anim.getAnimationTime() );
        }
        data->setKeyFrame( keyFrame );
        if( time != data->getTime() ) {
            data->m_time.merge( time );
        }
    }



    inline void fetchHandleAnimation( aiNodeAnim* node, HandleAnimation& data ) {
        const uint T_size = node->mNumPositionKeys;
        const uint R_size = node->mNumRotationKeys;
        const uint S_size = node->mNumScalingKeys;

        KeyTranslation tr;
        KeyRotation    rot;
        KeyScaling     s;

        data.m_anim.reset();
        std::set< Scalar > keyFrame;

        for( uint i = 0; i < T_size; ++i ) {
            Scalar               time  = node->mPositionKeys[i].mTime;
            aiVector3t< Scalar > value = node->mPositionKeys[i].mValue;
            tr.insertKey( time, assimpToCore( value ) );
            keyFrame.insert( time );
        }

        for( uint i = 0; i < R_size; ++i ) {
            Scalar                  time  = node->mRotationKeys[i].mTime;
            aiQuaterniont< Scalar > value = node->mRotationKeys[i].mValue;
            rot.insertKey( time, assimpToCore( value ) );
            keyFrame.insert( time );
        }

        for( uint i = 0; i < S_size; ++i ) {
            Scalar               time  = node->mScalingKeys[i].mTime;
            aiVector3t< Scalar > value = node->mScalingKeys[i].mValue;
            s.insertKey( time, assimpToCore( value ) );
            keyFrame.insert( time );
        }

        data.m_name = assimpToCore( node->mNodeName );
        data.m_anim.setAnimationTime( AnimationTime( *keyFrame.begin(), *keyFrame.rbegin() ) );
        for( const auto& time : keyFrame ) {
            Core::Transform T;
            T.fromPositionOrientationScale( tr.getKey( time ), rot.getKey( time ), s.getKey( time ) );
            data.m_anim.insertKey( time, T );
        }
    }


};


} // namespace Asset
} // namespace Ra

#endif // RADIUMENGINE_ASSIMP_ANIMATION_DATA_LOADER_HPP

#endif
