#include <Core/Math/Interpolation.hpp>
#include <IO/Gltf/Loader/glTFFileLoader.hpp>
#include <IO/Gltf/internal/GLTFConverter/TransformationManager.hpp>

using namespace Ra::Core;

namespace Ra {
namespace IO {
namespace GLTF {

void TransformationManager::insert( int32_t node,
                                    const std::string& path,
                                    float* times,
                                    float* transformations,
                                    int32_t count,
                                    fx::gltf::Animation::Sampler::Type /*interpolation*/,
                                    const std::array<float, 4>& nodeRotation,
                                    const std::array<float, 3>& nodeScale,
                                    const std::array<float, 3>& nodeTranslation ) {
    // FIXME : interpolation not use
    // weights' animation and scales' animation not handle by radium
    if ( path == "weights" ) {
        LOG( Ra::Core::Utils::logINFO )
            << "GLTF file contain weights animation. It's not handle by radium.";
        return;
    }

    // add times to m_times
    if ( m_times.find( node ) == m_times.end() ) {
        std::set<float> timesVec;
        std::map<float, Quaternionf> rotations;
        std::map<float, Vector3> scales;
        std::map<float, Vector3> translations;
        m_times.insert( std::pair( node, timesVec ) );
        m_rotation.insert( std::pair( node, rotations ) );
        m_scale.insert( std::pair( node, scales ) );
        m_translation.insert( std::pair( node, translations ) );
        m_nodeVisited.push_back( node );
        m_nodeBaseTransform.insert(
            std::pair( node, std::tuple( nodeRotation, nodeScale, nodeTranslation ) ) );
    }
    std::set<float>& timesVec = m_times[node];
    for ( int32_t i = 0; i < count; ++i ) {
        timesVec.insert( times[i] );
    }
    // insert times and transformations in map
    if ( path == "rotation" ) {
        std::map<float, Quaternionf>& rotations = m_rotation[node];
        for ( int32_t i = 0; i < count; ++i ) {
            Quaternionf quat( transformations[i * 4 + 3],
                              transformations[i * 4],
                              transformations[i * 4 + 1],
                              transformations[i * 4 + 2] );
            rotations.insert( std::pair( times[i], quat ) );
        }
        return;
    }
    if ( path == "scale" ) {
        std::map<float, Vector3>& scale = m_scale[node];
        for ( int32_t i = 0; i < count; ++i ) {
            Vector3 vec(
                transformations[i * 3], transformations[i * 3 + 1], transformations[i * 3 + 2] );
            scale.insert( std::pair( times[i], vec ) );
        }
        return;
    }
    if ( path == "translation" ) {
        std::map<float, Vector3>& translations = m_translation[node];
        for ( int32_t i = 0; i < count; ++i ) {
            Vector3f vec(
                transformations[i * 3], transformations[i * 3 + 1], transformations[i * 3 + 2] );
            translations.insert( std::pair( times[i], vec ) );
        }
        return;
    }
}

void TransformationManager::buildAnimation(
    std::vector<Ra::Core::Asset::HandleAnimation>& animations ) {
    for ( unsigned int node : m_nodeVisited ) {
        Ra::Core::Asset::HandleAnimation animation;
        std::map<float, Quaternionf>& rotations = m_rotation[node];
        std::map<float, Vector3>& scales        = m_scale[node];
        std::map<float, Vector3>& translations  = m_translation[node];

        // if there is no animation, then use Node's transform!
        if ( rotations.empty() ) {
            const auto& quat = std::get<0>( m_nodeBaseTransform[node] );
            Quaternionf quaternionf( quat[3], quat[0], quat[1], quat[2] );
            rotations.insert( std::pair<float, Quaternionf>( 0.0f, quaternionf ) );
        }
        if ( scales.empty() ) {
            const auto& vec = std::get<1>( m_nodeBaseTransform[node] );
            Vector3 vector3( vec[0], vec[1], vec[2] );
            scales.insert( std::pair<float, Vector3>( 0.0f, vector3 ) );
        }
        if ( translations.empty() ) {
            const auto& vec = std::get<2>( m_nodeBaseTransform[node] );
            Vector3 vector3( vec[0], vec[1], vec[2] );
            translations.insert( std::pair<float, Vector3>( 0.0f, vector3 ) );
        }

        auto it_rotation    = rotations.begin();
        auto it_scale       = scales.begin();
        auto it_translation = translations.begin();
        for ( float time : m_times[node] ) {
            // Rotation
            Quaternionf rotation;
            Vector3 scale;
            Vector3 translation;

            for ( ; it_rotation != rotations.end() && it_rotation->first < time; ++it_rotation )
                ;
            for ( ; it_scale != scales.end() && it_scale->first < time; ++it_scale )
                ;
            for ( ; it_translation != translations.end() && it_translation->first < time;
                  ++it_translation )
                ;

            if ( it_rotation == rotations.end() ) { rotation = prev( it_rotation )->second; }
            else if ( it_rotation->first == time || it_rotation == rotations.begin() ) {
                rotation = it_rotation->second;
            }
            else {
                float t = ( time - prev( it_rotation )->first ) /
                          ( it_rotation->first - prev( it_rotation )->first );
                rotation = Math::linearInterpolate(
                    prev( it_rotation, 1 )->second, it_rotation->second, t );
            }
            if ( it_scale == scales.end() ) { scale = prev( it_scale )->second; }
            else if ( it_scale->first == time || it_scale == scales.begin() ) {
                scale = it_scale->second;
            }
            else {
                float t = ( time - prev( it_scale )->first ) /
                          ( it_scale->first - prev( it_scale )->first );
                scale = Math::linearInterpolate( prev( it_scale, 1 )->second, it_scale->second, t );
            }
            if ( it_translation == translations.end() ) {
                translation = prev( it_translation )->second;
            }
            else if ( it_translation->first == time || it_translation == translations.begin() ) {
                translation = it_translation->second;
            }
            else {
                float t = ( time - prev( it_translation )->first ) /
                          ( it_translation->first - prev( it_translation )->first );
                translation = Math::linearInterpolate(
                    prev( it_translation, 1 )->second, it_translation->second, t );
            }

            Transform transform;
            transform.fromPositionOrientationScale( translation, rotation, scale );
            animation.m_anim.insertKeyFrame( time, transform );
        }
        animation.m_name = m_nodeIdToBoneName[node];
        animations.push_back( animation );
    }
}

} // namespace GLTF
} // namespace IO
} // namespace Ra
