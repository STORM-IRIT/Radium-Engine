#include <IO/AssimpLoader/AssimpLightDataLoader.hpp>

#include <assimp/scene.h>

#include <Core/Utils/Log.hpp>

#include <IO/AssimpLoader/AssimpWrapper.hpp>

namespace Ra {
namespace IO {

using namespace Core::Utils; // log

AssimpLightDataLoader::AssimpLightDataLoader( const std::string& filepath,
                                              const bool VERBOSE_MODE ) :
    DataLoader<Asset::LightData>( VERBOSE_MODE ),
    m_filepath( filepath ) {}

AssimpLightDataLoader::~AssimpLightDataLoader() = default;

/// LOADING
void AssimpLightDataLoader::loadData( const aiScene* scene,
                                      std::vector<std::unique_ptr<Asset::LightData>>& data ) {
    data.clear();

    if ( scene == nullptr )
    {
        LOG( logDEBUG ) << "AssimpLightDataLoader : scene is nullptr.";
        return;
    }

    if ( !sceneHasLight( scene ) )
    {
        LOG( logDEBUG ) << "AssimpLightDataLoader : scene has no lights.";
        return;
    }

    if ( m_verbose )
    {
        LOG( logINFO ) << "File contains light.";
        LOG( logINFO ) << "Light Loading begin...";
    }

    uint lightSize = sceneLightSize( scene );
    data.reserve( lightSize );
    for ( uint lightId = 0; lightId < lightSize; ++lightId )
    {
        data.emplace_back( loadLightData( scene, *( scene->mLights[lightId] ) ) );
    }

    if ( m_verbose )
    {
        LOG( logINFO ) << "Light Loading end.\n";
    }
}

bool AssimpLightDataLoader::sceneHasLight( const aiScene* scene ) const {
    return ( scene->HasLights() );
}

uint AssimpLightDataLoader::sceneLightSize( const aiScene* scene ) const {
    return scene->mNumLights;
}

// Asset::LightData * AssimpLightDataLoader::loadLightData(const aiScene *scene, const aiLight
// &light)
std::unique_ptr<Asset::LightData> AssimpLightDataLoader::loadLightData( const aiScene* scene,
                                                                        const aiLight& light ) {
    //    auto builtLight = new Asset::LightData(fetchName( light ), fetchType( light) );
    auto builtLight = std::make_unique<Asset::LightData>( fetchName( light ), fetchType( light ) );
    Core::Matrix4 rootMatrix;
    rootMatrix = Core::Matrix4::Identity();
    Core::Matrix4 frame = loadLightFrame( scene, rootMatrix, builtLight->getName() );
    setFrame( frame );
    auto color =
        Eigen::Map<const Eigen::Matrix<Scalar, 3, 1>>( &( light.mColorDiffuse.r ) ).homogeneous();

    switch ( builtLight->getType() )
    {
    case Asset::LightData::DIRECTIONAL_LIGHT:
    {
        Core::Vector4 dir( light.mDirection[0], light.mDirection[1], light.mDirection[2], 0.0 );
        builtLight->setLight( color, -( frame.transpose().inverse() * dir ).head<3>() );
    }
    break;

    case Asset::LightData::POINT_LIGHT:
    {
        builtLight->setLight(
            color,
            ( frame * Eigen::Map<const Eigen::Matrix<Scalar, 3, 1>>( &( light.mPosition.x ) )
                          .homogeneous() )
                .hnormalized(),
            Asset::LightData::LightAttenuation( light.mAttenuationConstant,
                                                light.mAttenuationLinear,
                                                light.mAttenuationQuadratic ) );
    }
    break;

    case Asset::LightData::SPOT_LIGHT:
    {
        Core::Vector4 dir( light.mDirection[0], light.mDirection[1], light.mDirection[2], 0.0 );

        builtLight->setLight(
            color,
            ( frame * Eigen::Map<const Eigen::Matrix<Scalar, 3, 1>>( &( light.mPosition.x ) )
                          .homogeneous() )
                .hnormalized(),
            -( frame.transpose().inverse() * dir ).head<3>(), light.mAngleInnerCone,
            light.mAngleOuterCone,
            Asset::LightData::LightAttenuation( light.mAttenuationConstant,
                                                light.mAttenuationLinear,
                                                light.mAttenuationQuadratic ) );
    }
    break;

    case Asset::LightData::AREA_LIGHT:
    {
        LOG( logWARNING ) << "Light " << builtLight->getName()
                          << " : AREA light are not yet supported.";
    }
    break;
    default:
    { LOG( logWARNING ) << "Light " << builtLight->getName() << " : unknown type."; }
    break;
    }
    return builtLight;
}

Core::Matrix4 AssimpLightDataLoader::loadLightFrame( const aiScene* scene,
                                                     const Core::Matrix4& parentFrame,
                                                     const std::string& lightName ) const {
    const aiNode* lightNode = scene->mRootNode->FindNode( lightName.c_str() );

    if ( lightNode != nullptr )
    {

        auto t0 = Core::Matrix4::NullaryExpr(
            [&scene]( int i, int j ) { return scene->mRootNode->mTransformation[i][j]; } );
        auto t1 = Core::Matrix4::NullaryExpr(
            [&lightNode]( int i, int j ) { return lightNode->mTransformation[i][j]; } );

        return parentFrame * t0 * t1;
    }
    return parentFrame;
}

std::string AssimpLightDataLoader::fetchName( const aiLight& light ) const {
    return assimpToCore( light.mName );
}

Asset::LightData::LightType AssimpLightDataLoader::fetchType( const aiLight& light ) const {
    switch ( light.mType )
    {
    case aiLightSource_DIRECTIONAL:
    { return Asset::LightData::DIRECTIONAL_LIGHT; }

    case aiLightSource_POINT:
    { return Asset::LightData::POINT_LIGHT; }

    case aiLightSource_SPOT:
    { return Asset::LightData::SPOT_LIGHT; }

    case aiLightSource_AREA:
    { return Asset::LightData::AREA_LIGHT; }

    case aiLightSource_UNDEFINED:
    default:
    {
        //                LOG(ERROR) << "Light " << name.C_Str() << " has undefined type.";
        return Asset::LightData::UNKNOWN;
    }
    }
}

} // namespace IO
} // namespace Ra
