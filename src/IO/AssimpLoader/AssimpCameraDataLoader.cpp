#include <IO/AssimpLoader/AssimpCameraDataLoader.hpp>

#include <assimp/scene.h>

#include <Core/File/CameraData.hpp>
#include <Core/Log/Log.hpp>

#include <IO/AssimpLoader/AssimpWrapper.hpp>

namespace Ra {
namespace IO {

AssimpCameraDataLoader::AssimpCameraDataLoader( const std::string& filepath,
                                                const bool VERBOSE_MODE ) :
    DataLoader<Asset::CameraData>( VERBOSE_MODE ) {}

AssimpCameraDataLoader::~AssimpCameraDataLoader() = default;

void AssimpCameraDataLoader::loadData( const aiScene* scene,
                                       std::vector<std::unique_ptr<Asset::CameraData>>& data ) {
    data.clear();

    if ( scene == nullptr )
    {
        LOG( logDEBUG ) << "AssimpCameraDataLoader : scene is nullptr.";
        return;
    }

    if ( !sceneHasCamera( scene ) )
    {
        LOG( logDEBUG ) << "AssimpCameraDataLoader : scene has no Cameras.";
        return;
    }

    if ( m_verbose )
    {
        LOG( logINFO ) << "File contains Camera.";
        LOG( logINFO ) << "Camera Loading begin...";
    }

    uint CameraSize = sceneCameraSize( scene );
    data.reserve( CameraSize );
    for ( uint CameraId = 0; CameraId < CameraSize; ++CameraId )
    {
        Asset::CameraData* Camera = new Asset::CameraData();
        loadCameraData( scene, *( scene->mCameras[CameraId] ), *Camera );
        data.push_back( std::unique_ptr<Asset::CameraData>( Camera ) );
    }

    if ( m_verbose )
    {
        LOG( logINFO ) << "Camera Loading end.\n";
    }
}

bool AssimpCameraDataLoader::sceneHasCamera( const aiScene* scene ) const {
    return ( scene->HasCameras() );
}

uint AssimpCameraDataLoader::sceneCameraSize( const aiScene* scene ) const {
    return scene->mNumCameras;
}

void AssimpCameraDataLoader::loadCameraData( const aiScene* scene, const aiCamera& camera,
                                             Asset::CameraData& data ) {
    fetchName( camera, data );

    Core::Matrix4 rootMatrix;
    rootMatrix = Core::Matrix4::Identity();
    Core::Matrix4 frame = loadCameraFrame( scene, rootMatrix, data );
    Core::Vector3 pos = assimpToCore( camera.mPosition );
    Core::Vector3 lookAt = assimpToCore( camera.mLookAt ).normalized();
    Core::Vector3 up = assimpToCore( camera.mUp ).normalized();
    Core::Vector3 right = lookAt.cross( up );
    Core::Matrix4 view;
    view.block<3, 1>( 0, 0 ) = right;
    view.block<3, 1>( 0, 1 ) = up;
    view.block<3, 1>( 0, 2 ) = lookAt;
    view.block<3, 1>( 0, 3 ) = pos;
    data.setFrame( view * frame ); // TODO: check that!!!

    data.setType( Asset::CameraData::PERSPECTIVE ); // default value since not in aiCamera
    data.setFov( camera.mHorizontalFOV );
    data.setZNear( camera.mClipPlaneNear );
    data.setZFar( camera.mClipPlaneFar );
    data.setZoomFactor( 1.0 ); // default value since not in aiCamera
    data.setAspect( camera.mAspect );
}

Core::Matrix4 AssimpCameraDataLoader::loadCameraFrame( const aiScene* scene,
                                                       const Core::Matrix4& parentFrame,
                                                       Asset::CameraData& data ) const {
    const aiNode* CameraNode = scene->mRootNode->FindNode( data.getName().c_str() );
    Core::Matrix4 transform;
    transform = Core::Matrix4::Identity();

    if ( CameraNode != nullptr )
    {
        Core::Matrix4 t0;
        Core::Matrix4 t1;

        for ( uint i = 0; i < 4; ++i )
        {
            for ( uint j = 0; j < 4; ++j )
            {
                t0( i, j ) = scene->mRootNode->mTransformation[i][j];
                t1( i, j ) = CameraNode->mTransformation[i][j];
            }
        }
        transform = t0 * t1;
    }

    return parentFrame * transform;
}

void AssimpCameraDataLoader::fetchName( const aiCamera& camera, Asset::CameraData& data ) const {
    std::string name = assimpToCore( camera.mName );
    data.setName( name );
}

} // namespace IO
} // namespace Ra
