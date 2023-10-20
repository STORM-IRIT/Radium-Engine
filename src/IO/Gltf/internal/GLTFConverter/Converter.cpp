#include <Core/Asset/FileData.hpp>
#include <IO/Gltf/internal/GLTFConverter/AccessorReader.hpp>
#include <IO/Gltf/internal/GLTFConverter/Converter.hpp>
#include <IO/Gltf/internal/GLTFConverter/HandleData.hpp>
#include <IO/Gltf/internal/GLTFConverter/MeshData.hpp>
#include <IO/Gltf/internal/GLTFConverter/TransformationManager.hpp>

#include <IO/Gltf/internal/Extensions/LightExtensions.hpp>

using namespace fx;

namespace Ra {
namespace IO {
namespace GLTF {

using namespace Ra::Core;
using namespace Ra::Core::Asset;
using namespace Ra::Core::Utils;

static std::vector<std::string> gltfSupportedExtensions { { "KHR_lights_punctual" },
                                                          { "KHR_materials_pbrSpecularGlossiness" },
                                                          { "KHR_texture_transform" },
                                                          { "KHR_materials_ior" },
                                                          { "KHR_materials_clearcoat" },
                                                          { "KHR_materials_specular" },
                                                          { "KHR_materials_sheen" },
                                                          { "KHR_materials_unlit" } };

// Check extensions used or required by this gltf scene
bool checkExtensions( const gltf::Document& gltfscene ) {

    if ( !gltfscene.extensionsRequired.empty() ) {
        for ( const auto& ext : gltfscene.extensionsRequired ) {
            if ( !std::any_of( gltfSupportedExtensions.begin(),
                               gltfSupportedExtensions.end(),
                               [&ext]( const auto& supported ) { return supported == ext; } ) ) {
                LOG( logINFO ) << "Required extension " << ext
                               << " not supported by Radium glTF2 file loader.";
                return false;
            }
        }
    }
    if ( !gltfscene.extensionsUsed.empty() ) {
        for ( const auto& ext : gltfscene.extensionsUsed ) {
            if ( !std::any_of( gltfSupportedExtensions.begin(),
                               gltfSupportedExtensions.end(),
                               [&ext]( const auto& supported ) { return supported == ext; } ) ) {
                LOG( logINFO )
                    << "Used extension " << ext
                    << " not supported by Radium glTF2 file loader, fallback to default.";
            }
            else { LOG( logINFO ) << "Using extension " << ext; }
        }
    }
    return true;
}

LightData*
getLight( const gltf_KHR_lights_punctual& lights, int32_t lightIndex, const Transform& transform ) {
    if ( lightIndex < int32_t( lights.lights.size() ) ) {
        const auto gltfLight  = lights.lights[lightIndex];
        std::string lightName = gltfLight.name;
        if ( lightName.empty() ) { lightName = "light_" + std::to_string( lightIndex ); }
        auto color = Ra::Core::Utils::Color { gltfLight.color[0] * gltfLight.intensity,
                                              gltfLight.color[1] * gltfLight.intensity,
                                              gltfLight.color[2] * gltfLight.intensity };

        auto radiumLight = new LightData( lightName );
        // warning, as this property is not used by Radium LightSystem (lightManager),
        // must pre-transform the data
        radiumLight->setFrame( transform.matrix() );
        auto pos = Ra::Core::Vector3 { 0_ra, 0_ra, 0_ra };
        pos      = transform * pos;
        auto dir = Ra::Core::Vector3 { 0_ra, 0_ra, -1_ra };
        dir      = transform.linear() * dir;
        LightData::LightAttenuation attenuation { 0_ra, 0_ra, 1_ra };
        switch ( gltfLight.type ) {
        case gltf_lightPunctual::Type::directional:
            radiumLight->setLight( color, dir );
            break;
        case gltf_lightPunctual::Type::point:
            radiumLight->setLight( color, pos, attenuation );
            break;
        case gltf_lightPunctual::Type::spot:
            radiumLight->setLight( color,
                                   pos,
                                   dir,
                                   gltfLight.spot.innerConeAngle,
                                   gltfLight.spot.outerConeAngle,
                                   attenuation );
            break;
        default:
            delete radiumLight;
            radiumLight = nullptr;
            break;
        }
        return radiumLight;
    }
    LOG( logWARNING ) << "Gltf loader : request for light " << lightIndex << " but only "
                      << lights.lights.size() << " lights are available.";
    return nullptr;
}

Camera* buildCamera( const gltf::Document& doc,
                     int32_t cameraIndex,
                     const Transform& parentTransform,
                     const std::string& /*filePath*/,
                     int32_t nodeNum ) {
    // Radium Camera have problems if there is a scaling in the matrix : remove the scaling
    // TODO : verify and check against the gltf specification
    Ra::Core::Matrix4 cameraTransform = parentTransform.matrix();
    cameraTransform.block( 0, 0, 3, 1 ).normalize();
    cameraTransform.block( 0, 1, 3, 1 ).normalize();
    cameraTransform.block( 0, 2, 3, 1 ).normalize();

    switch ( doc.cameras[cameraIndex].type ) {
    case gltf::Camera::Type::Orthographic: {
        auto cam = doc.cameras[cameraIndex].orthographic;
        // todo test if cam is empty ?
        auto name = doc.cameras[cameraIndex].name;
        if ( name.empty() ) { name = std::string { "Cam_gltf_" } + std::to_string( nodeNum ); }
        else { name += std::string { "Cam_" } + std::to_string( nodeNum ); }
        auto radiumCam = new Camera();
        radiumCam->setType( Camera::ProjType::ORTHOGRAPHIC );
        radiumCam->setFrame( Transform { cameraTransform } );
        radiumCam->setZNear( cam.znear );
        radiumCam->setZFar( cam.zfar );
        radiumCam->setXYmag( cam.xmag, cam.ymag );
        radiumCam->setViewport( 1_ra, 1_ra ); // TODO check this
        return radiumCam;
    }
    case gltf::Camera::Type::Perspective: {
        auto cam = doc.cameras[cameraIndex].perspective;
        // todo test if cam is empty ?
        auto name = doc.cameras[cameraIndex].name;
        if ( name.empty() ) { name = std::string { "Cam_gltf_" } + std::to_string( nodeNum ); }
        else { name += std::string { "Cam_" } + std::to_string( nodeNum ); }
        auto radiumCam = new Camera();
        radiumCam->setType( Camera::ProjType::PERSPECTIVE );
        radiumCam->setZNear( cam.znear );
        radiumCam->setZFar( cam.zfar );

        if ( cam.aspectRatio > 0 ) { radiumCam->setViewport( radiumCam->getAspect(), 1_ra ); }
        else { radiumCam->setViewport( 1_ra, 1_ra ); }

        // convert fovy to fovx
        Scalar fovxDiv2 = std::atan( radiumCam->getAspect() * std::tan( cam.yfov / 2_ra ) );

        // consider that fov < pi. if fovy/2 is more than pi/2 (i.e. atan return <0)
        // let's clamp it to pi/2
        if ( fovxDiv2 < 0_ra ) { fovxDiv2 = Ra::Core::Math::PiDiv2; }

        radiumCam->setFOV( 2_ra * fovxDiv2 );

        radiumCam->setFrame( Transform { cameraTransform } );
        return radiumCam;
    }
    default:
        return nullptr;
    }
}

// Compute the combined matrix at each node
void glTfVisitor( const gltf::Document& scene,
                  int32_t nodeIndex,
                  const Transform& parentTransform,
                  std::vector<SceneNode>& graphNodes,
                  std::set<int32_t>& visitedNodes ) {
    SceneNode& graphNode  = graphNodes[nodeIndex];
    graphNode.m_transform = parentTransform;
    visitedNodes.insert( nodeIndex );
    gltf::Node const& node = scene.nodes[nodeIndex];
    if ( !node.name.empty() ) { graphNode.m_nodeName = node.name; }
    else { graphNode.m_nodeName = "Unnamed node"; }
    if ( node.matrix != gltf::defaults::IdentityMatrix ) {
        auto tr = node.matrix.data();
        Matrix4f mat;
        mat << tr[0], tr[4], tr[8], tr[12], tr[1], tr[5], tr[9], tr[13], tr[2], tr[6], tr[10],
            tr[14], tr[3], tr[7], tr[11], tr[15];
        graphNode.m_transform = graphNode.m_transform * Transform( mat );
    }
    else {
        // gltf transform is T * R * S
        if ( node.translation != gltf::defaults::NullVec3 ) {
            auto tr = node.translation.data();
            graphNode.m_transform.translate( Vector3( tr[0], tr[1], tr[2] ) );
        }
        if ( node.rotation != gltf::defaults::IdentityVec4 ) {
            auto tr = node.rotation.data();
            Quaternionf quat( tr[3], tr[0], tr[1], tr[2] );
            graphNode.m_transform.rotate( quat );
        }
        if ( node.scale != gltf::defaults::IdentityVec3 ) {
            auto tr = node.scale.data();
            graphNode.m_transform.scale( Vector3( tr[0], tr[1], tr[2] ) );
        }
    }

    if ( node.camera >= 0 ) {
        graphNode.m_cameraIndex = node.camera;
        /*
        LOG( logINFO ) << "Camera node with transformation : \n"
                       << graphNode.m_transform.matrix() << std::endl;
        */
    }
    else {
        if ( node.mesh >= 0 ) {
            graphNode.m_meshIndex = node.mesh;
            if ( node.skin >= 0 ) { graphNode.m_skinIndex = node.skin; }
        }
        else { graphNode.initPropsFromExtensionsAndExtra( node.extensionsAndExtras ); }

        graphNode.children = node.children;
        for ( auto childIndex : graphNode.children ) {
            glTfVisitor( scene, childIndex, graphNode.m_transform, graphNodes, visitedNodes );
        }
    }
}

void buildAnimation( std::vector<HandleAnimation>& animations,
                     HandleDataLoader::IntToString& map,
                     const fx::gltf::Document& doc,
                     int activeAnimation ) {
    std::vector<gltf::Animation::Channel> channels = doc.animations[activeAnimation].channels;
    std::vector<gltf::Animation::Sampler> samplers = doc.animations[activeAnimation].samplers;
    AccessorReader accessorReader( doc );
    TransformationManager transformationManager( map );
    for ( const gltf::Animation::Channel& channel : channels ) {
        gltf::Animation::Channel::Target target = channel.target;
        gltf::Animation::Sampler sampler        = samplers[channel.sampler];
        // weights' and scale's animations not handle by radium  =>  now it does, so what is that
        // comment for?
        auto times          = reinterpret_cast<float*>( accessorReader.read( sampler.input ) );
        auto transformation = reinterpret_cast<float*>( accessorReader.read( sampler.output ) );
        transformationManager.insert( target.node,
                                      target.path,
                                      times,
                                      transformation,
                                      doc.accessors[sampler.input].count,
                                      sampler.interpolation,
                                      doc.nodes[channel.target.node].rotation,
                                      doc.nodes[channel.target.node].scale,
                                      doc.nodes[channel.target.node].translation );
    }
    transformationManager.buildAnimation( animations );
}

Converter::Converter( FileData* fd, const std::string& baseDir ) :
    fileData { fd }, filePath { baseDir } {}

bool Converter::operator()( const gltf::Document& gltfscene ) {

    MeshNameCache::resetCache();

    if ( !checkExtensions( gltfscene ) ) { return false; }
    // pre-load supported scene extensions
    // manage node extension
    // KHR_lights_punctual;
    gltf_KHR_lights_punctual gltfLights;

    if ( !gltfscene.extensionsAndExtras.empty() ) {
        auto extensions = gltfscene.extensionsAndExtras.find( "extensions" );
        if ( extensions != gltfscene.extensionsAndExtras.end() ) {
            auto iter = extensions->find( "KHR_lights_punctual" );
            if ( iter != extensions->end() ) {
                from_json( *iter, gltfLights );
                LOG( logINFO ) << "Found KHR_lights_punctual extension with "
                               << gltfLights.lights.size() << " light sources.";
            }
        }
    }
    // cf https://github.com/MathiasPaulin/fx-gltf/blob/master/examples/viewer/DirectX/D3DEngine.cpp
    // BuildScene
    if ( !gltfscene.scenes.empty() ) {
        std::vector<SceneNode> graphNodes( gltfscene.nodes.size() );

        Transform rootTransform;
        rootTransform.setIdentity();

        int activeScene = gltfscene.scene;
        if ( activeScene == -1 ) activeScene = 0;
        std::set<int32_t> visitedNodes;
        for ( auto sceneNode : gltfscene.scenes[activeScene].nodes ) {
            glTfVisitor( gltfscene, sceneNode, rootTransform, graphNodes, visitedNodes );
        }

        int32_t nodeNum = 0;

        // build Radium representation of the scene elements
        for ( auto visited : visitedNodes ) {
            auto& graphNode = graphNodes[visited];
            // Is the node a mesh ?
            if ( graphNode.m_meshIndex >= 0 ) {
                auto meshParts = buildMesh( gltfscene, graphNode.m_meshIndex, filePath, nodeNum );
                for ( auto& p : meshParts ) {
                    p->setFrame( graphNode.m_transform );
                    fileData->m_geometryData.emplace_back( std::move( p ) );
                }
                ++nodeNum;
            }
            // Is the node a Camera ?
            if ( graphNode.m_cameraIndex >= 0 ) {

                fileData->m_cameraData.emplace_back( buildCamera( gltfscene,
                                                                  graphNode.m_cameraIndex,
                                                                  graphNode.m_transform,
                                                                  filePath,
                                                                  nodeNum ) );

                ++nodeNum;
            }
            // Is the node a Light ?
            if ( graphNode.m_lightIndex >= 0 ) {
                auto radiumLight =
                    getLight( gltfLights, graphNode.m_lightIndex, graphNode.m_transform );
                if ( radiumLight ) { fileData->m_lightData.emplace_back( radiumLight ); }
            }
        }

        // build Radium Skeletons
        HandleDataLoader::IntToString nodeNumToComponentName;
        if ( !gltfscene.skins.empty() ) {
            size_t skinIndex = 0;
            for ( const auto& skin : gltfscene.skins ) {
                auto skeleton = HandleDataLoader::loadSkeleton(
                    gltfscene, graphNodes, visitedNodes, skin, skinIndex, nodeNumToComponentName );
                fileData->m_handleData.push_back( std::unique_ptr<HandleData>( skeleton ) );
                if ( fileData->isVerbose() ) { skeleton->displayInfo(); }
                ++skinIndex;
            }
            LOG( logINFO ) << "Loaded " << skinIndex << " skeletons.";
        }

        // BuildAnimation
        if ( !gltfscene.animations.empty() ) {
            int activeAnimation = 0;
            // find the first animation that affect the scene
            while ( activeAnimation < int( gltfscene.animations.size() ) &&
                    ( visitedNodes.find(
                          gltfscene.animations[activeAnimation].channels[0].target.node ) ==
                      visitedNodes.end() ) ) {
                ++activeAnimation;
            }
            // if animation found
            if ( activeAnimation < int( gltfscene.animations.size() ) ) {
                auto animationData = new AnimationData();
                // set m_dt
                animationData->setTimeStep( 1.0f / 60.0f );
                // set m_keyframe
                std::vector<HandleAnimation> animationPart;
                buildAnimation( animationPart, nodeNumToComponentName, gltfscene, activeAnimation );
                animationData->setHandleData( animationPart );
                // set m_time
                Asset::AnimationTime time;
                time.setStart( 0.f );
                time.setEnd( 0.f );
                for ( const auto& handleAnimation : animationPart ) {
                    auto t = handleAnimation.m_animationTime;
                    time.extends( t );
                }
                animationData->setTime( time );
                // set m_name
                if ( !gltfscene.animations[activeAnimation].name.empty() ) {
                    animationData->setName( gltfscene.animations[activeAnimation].name );
                }
                else { animationData->setName( { "Animation_defaultname" } ); }
                fileData->m_animationData.push_back(
                    std::unique_ptr<AnimationData>( animationData ) );
            }
        }
    }
    else {
        Transform rootTransform;
        rootTransform.setIdentity();
        for ( uint32_t i = 0; i < gltfscene.meshes.size(); i++ ) {
            auto meshParts = buildMesh( gltfscene, i, filePath, i );
            for ( auto& p : meshParts ) {
                p->setFrame( rootTransform );
                fileData->m_geometryData.emplace_back( std::move( p ) );
            }
        }
    }

    MeshNameCache::resetCache();

    if ( fileData->isVerbose() ) {
        LOG( logINFO ) << "Loaded gltf file : \n\t" << gltfscene.asset.generator << "\n\t"
                       << gltfscene.asset.copyright << "\n\tVersion " << gltfscene.asset.version;
        // move the following in the verbose part
        LOG( logINFO ) << "Loaded file contains : " << std::endl;
        LOG( logINFO ) << "\t" << gltfscene.accessors.size() << " accessors.";
        LOG( logINFO ) << "\t" << gltfscene.animations.size() << " animations.";
        LOG( logINFO ) << "\t" << gltfscene.buffers.size() << " buffers.";
        LOG( logINFO ) << "\t" << gltfscene.bufferViews.size() << " bufferViews.";
        LOG( logINFO ) << "\t" << gltfscene.cameras.size() << " cameras.";
        LOG( logINFO ) << "\t" << gltfscene.images.size() << " images.";
        LOG( logINFO ) << "\t" << gltfscene.materials.size() << " materials.";
        LOG( logINFO ) << "\t" << gltfscene.meshes.size() << " meshes.";
        LOG( logINFO ) << "\t" << gltfscene.nodes.size() << " nodes.";
        LOG( logINFO ) << "\t" << gltfscene.samplers.size() << " samplers.";
        LOG( logINFO ) << "\t" << gltfscene.scenes.size() << " scenes.";
        LOG( logINFO ) << "\t" << gltfscene.skins.size() << " skins.";
        LOG( logINFO ) << "\t" << gltfscene.textures.size() << " textures.";
        LOG( logINFO ) << "Active scene is  : " << gltfscene.scene;
        if ( gltfscene.scene >= 0 ) {
            LOG( logINFO ) << "\t" << gltfscene.scenes[gltfscene.scene].name << std::endl;
        }
    }
    return true;
}

} // namespace GLTF
} // namespace IO
} // namespace Ra
