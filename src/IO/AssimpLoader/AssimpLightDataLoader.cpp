#include <IO/AssimpLoader/AssimpLightDataLoader.hpp>

#include <assimp/scene.h>

#include <Core/File/LightData.hpp>
#include <Core/Log/Log.hpp>

#include <Engine/Renderer/Light/DirLight.hpp>
#include <Engine/Renderer/Light/Light.hpp>
#include <Engine/Renderer/Light/PointLight.hpp>
#include <Engine/Renderer/Light/SpotLight.hpp>

#include <IO/AssimpLoader/AssimpWrapper.hpp>

namespace Ra {
namespace IO {

AssimpLightDataLoader::AssimpLightDataLoader( const std::string& filepath,
                                              const bool VERBOSE_MODE ) :
    DataLoader<Asset::LightData>( VERBOSE_MODE ),
    m_filepath( filepath ) {}

AssimpLightDataLoader::~AssimpLightDataLoader() {}

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
        Asset::LightData* light = new Asset::LightData();
        loadLightData( scene, *( scene->mLights[lightId] ), *light );
        data.push_back( std::unique_ptr<Asset::LightData>( light ) );
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

void AssimpLightDataLoader::loadLightData( const aiScene* scene, const aiLight& light,
                                           Asset::LightData& data ) {
    fetchName( light, data );
    fetchType( light, data );
    Core::Matrix4 rootMatrix;
    rootMatrix = Core::Matrix4::Identity();
    Core::Matrix4 frame = loadLightFrame( scene, rootMatrix, data );
    setFrame( frame );
    Core::Color color( light.mColorDiffuse.r, light.mColorDiffuse.g, light.mColorDiffuse.b, 1.0 );

    switch ( data.getType() )
    {
    case Asset::LightData::DIRECTIONAL_LIGHT:
    {
        Core::Vector4 dir( light.mDirection[0], light.mDirection[1], light.mDirection[2], 0.0 );
        dir = frame.transpose().inverse() * dir;

        Core::Vector3 finalDir( dir.x(), dir.y(), dir.z() );
        finalDir = -finalDir;

        auto thelight = std::shared_ptr<Engine::DirectionalLight>( new Engine::DirectionalLight() );
        thelight->setColor( color );
        thelight->setDirection( finalDir );
        data.setLight( thelight );
    }
    break;

    case Asset::LightData::POINT_LIGHT:
    {
        Core::Vector4 pos( light.mPosition[0], light.mPosition[1], light.mPosition[2], 1.0 );
        pos = frame * pos;
        pos /= pos.w();

        auto thelight = std::shared_ptr<Engine::PointLight>( new Engine::PointLight() );
        thelight->setColor( color );
        thelight->setPosition( Core::Vector3( pos.x(), pos.y(), pos.z() ) );
        thelight->setAttenuation( light.mAttenuationConstant, light.mAttenuationLinear,
                                  light.mAttenuationQuadratic );
        data.setLight( thelight );
    }
    break;

    case Asset::LightData::SPOT_LIGHT:
    {
        Core::Vector4 pos( light.mPosition[0], light.mPosition[1], light.mPosition[2], 1.0 );
        pos = frame * pos;
        pos /= pos.w();

        Core::Vector4 dir( light.mDirection[0], light.mDirection[1], light.mDirection[2], 0.0 );
        dir = frame.transpose().inverse() * dir;

        Core::Vector3 finalDir( dir.x(), dir.y(), dir.z() );
        finalDir = -finalDir;

        auto thelight = std::shared_ptr<Engine::SpotLight>( new Engine::SpotLight() );
        thelight->setColor( color );
        thelight->setPosition( Core::Vector3( pos.x(), pos.y(), pos.z() ) );
        thelight->setDirection( finalDir );
        thelight->setAttenuation( light.mAttenuationConstant, light.mAttenuationLinear,
                                  light.mAttenuationQuadratic );
        thelight->setInnerAngleInRadians( light.mAngleInnerCone );
        thelight->setOuterAngleInRadians( light.mAngleOuterCone );
        data.setLight( thelight );
    }
    break;

    case Asset::LightData::AREA_LIGHT:
    { LOG( logWARNING ) << "Light " << data.getName() << " : AREA light are not yet supported."; }
    break;
    default:
    { LOG( logWARNING ) << "Light " << data.getName() << " : unknown type."; }
    break;
    }
}

Core::Matrix4 AssimpLightDataLoader::loadLightFrame( const aiScene* scene,
                                                     const Core::Matrix4& parentFrame,
                                                     Asset::LightData& data ) const {
    const aiNode* lightNode = scene->mRootNode->FindNode( data.getName().c_str() );
    Core::Matrix4 transform;
    transform = Core::Matrix4::Identity();

    if ( lightNode != nullptr )
    {
        Core::Matrix4 t0;
        Core::Matrix4 t1;

        for ( uint i = 0; i < 4; ++i )
        {
            for ( uint j = 0; j < 4; ++j )
            {
                t0( i, j ) = scene->mRootNode->mTransformation[i][j];
                t1( i, j ) = lightNode->mTransformation[i][j];
            }
        }
        transform = t0 * t1;
    }
    return parentFrame * transform;
}

void AssimpLightDataLoader::fetchName( const aiLight& light, Asset::LightData& data ) const {
    std::string name = assimpToCore( light.mName );
    data.setName( name );
}

void AssimpLightDataLoader::fetchType( const aiLight& light, Asset::LightData& data ) const {
    data.setType( Asset::LightData::UNKNOWN );
    switch ( light.mType )
    {
    case aiLightSource_DIRECTIONAL:
    { data.setType( Asset::LightData::DIRECTIONAL_LIGHT ); }
    break;

    case aiLightSource_POINT:
    { data.setType( Asset::LightData::POINT_LIGHT ); }
    break;

    case aiLightSource_SPOT:
    { data.setType( Asset::LightData::SPOT_LIGHT ); }
    break;
    case aiLightSource_AREA:
    { data.setType( Asset::LightData::AREA_LIGHT ); }
    break;
    case aiLightSource_UNDEFINED:
    default:
    {
        //                LOG(ERROR) << "Light " << name.C_Str() << " has undefined type.";
    }
    break;
    }
}

} // namespace IO
} // namespace Ra
