#include <IO/AssimpLoader/AssimpCameraDataLoader.hpp>

#include <assimp/scene.h>

#include <Core/Asset/CameraData.hpp>
#include <Core/Utils/Log.hpp>

#include <IO/AssimpLoader/AssimpWrapper.hpp>

namespace Ra {
namespace IO {

using namespace Core::Utils; // log
using namespace Core::Asset; // log

AssimpCameraDataLoader::AssimpCameraDataLoader( const bool VERBOSE_MODE ) :
    DataLoader<CameraData>( VERBOSE_MODE ) {}

AssimpCameraDataLoader::~AssimpCameraDataLoader() = default;

void AssimpCameraDataLoader::loadData( const aiScene* scene,
                                       std::vector<std::unique_ptr<CameraData>>& data ) {
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
        CameraData* Camera = new CameraData();
        loadCameraData( scene, *( scene->mCameras[CameraId] ), *Camera );
        data.push_back( std::unique_ptr<CameraData>( Camera ) );
    }

    if ( m_verbose ) { LOG( logINFO ) << "Camera Loading end.\n"; }
}

bool AssimpCameraDataLoader::sceneHasCamera( const aiScene* scene ) const {
    return ( scene->HasCameras() );
}

uint AssimpCameraDataLoader::sceneCameraSize( const aiScene* scene ) const {
    return scene->mNumCameras;
}

void AssimpCameraDataLoader::loadCameraData( const aiScene* scene,
                                             const aiCamera& camera,
                                             CameraData& data ) {
    fetchName( camera, data );

    Core::Matrix4 rootMatrix;
    rootMatrix           = Core::Matrix4::Identity();
    Core::Matrix4 frame  = loadCameraFrame( scene, rootMatrix, data );
    Core::Vector3 pos    = assimpToCore( camera.mPosition );
    Core::Vector3 lookAt = assimpToCore( camera.mLookAt ).normalized();
    Core::Vector3 up     = assimpToCore( camera.mUp ).normalized();
    Core::Vector3 right  = lookAt.cross( up );
    Core::Matrix4 view;
    view.block<3, 1>( 0, 0 ) = right;
    view.block<3, 1>( 0, 1 ) = up;
    view.block<3, 1>( 0, 2 ) = lookAt;
    view.block<3, 1>( 0, 3 ) = pos;
    data.setFrame( view * frame );

    data.setType( CameraData::PERSPECTIVE ); // default value since not in aiCamera
    data.setFov( camera.mHorizontalFOV );
    data.setZNear( camera.mClipPlaneNear );
    data.setZFar( camera.mClipPlaneFar );
    data.setZoomFactor( 1.0 ); // default value since not in aiCamera
    data.setAspect( camera.mAspect );
}

Core::Matrix4 AssimpCameraDataLoader::loadCameraFrame( const aiScene* scene,
                                                       const Core::Matrix4& parentFrame,
                                                       CameraData& data ) const {
    const aiNode* CameraNode = scene->mRootNode->FindNode( data.getName().c_str() );

    if ( CameraNode != nullptr )
    {
        auto t0 = Core::Matrix4::NullaryExpr(
            [&scene]( int i, int j ) { return scene->mRootNode->mTransformation[i][j]; } );
        auto t1 = Core::Matrix4::NullaryExpr(
            [&CameraNode]( int i, int j ) { return CameraNode->mTransformation[i][j]; } );

        return parentFrame * t0 * t1;
    }
    else
    { return parentFrame; }
}

void AssimpCameraDataLoader::fetchName( const aiCamera& camera, CameraData& data ) const {
    std::string name = assimpToCore( camera.mName );
    data.setName( name );
}

} // namespace IO
} // namespace Ra
