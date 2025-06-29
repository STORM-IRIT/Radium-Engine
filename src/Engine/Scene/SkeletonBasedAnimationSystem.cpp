#include <Core/Animation/KeyFramedValue.hpp>
#include <Core/Animation/Skeleton.hpp>
#include <Core/Asset/FileData.hpp>
#include <Core/Asset/GeometryData.hpp>
#include <Core/Asset/HandleData.hpp>
#include <Core/Math/Math.hpp>
#include <Core/Resources/Resources.hpp>
#include <Core/Tasks/Task.hpp>
#include <Core/Tasks/TaskQueue.hpp>
#include <Engine/Data/Texture.hpp>
#include <Engine/Data/TextureManager.hpp>
#include <Engine/FrameInfo.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Scene/Component.hpp>
#include <Engine/Scene/SkeletonBasedAnimationSystem.hpp>
#include <Engine/Scene/SkeletonComponent.hpp>
#include <Engine/Scene/SkinningComponent.hpp>
#include <algorithm>
#include <functional>
#include <limits>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace Ra {
namespace Engine {
namespace Scene {
class Entity;
} // namespace Scene
} // namespace Engine
} // namespace Ra

using namespace Ra::Core::Animation;

namespace Ra {
namespace Engine {
namespace Scene {

SkeletonBasedAnimationSystem::SkeletonBasedAnimationSystem() : System(), m_xrayOn( false ) {
    auto resourceDir { Core::Resources::getRadiumResourcesPath() };
    if ( resourceDir ) {
        auto* engine  = RadiumEngine::getInstance();
        auto* texMngr = engine->getTextureManager();
        // Register an entry into the texture manager
        // load the texture image without OpenGL initialization
        auto image = texMngr->loadTextureImage( *resourceDir + "/Textures/heatmap.png" );
        Data::TextureParameters heatMapTexturePamameters = { "Engine:Skinning:weights", {}, image };
        m_heatMapTextureHandle = texMngr->addTexture( heatMapTexturePamameters );
    }
}

SkeletonBasedAnimationSystem::~SkeletonBasedAnimationSystem() {
    SkeletonComponent::s_boneMesh.reset();
    SkeletonComponent::s_boneMaterial.reset();
    SkeletonComponent::s_boneRenderTechnique.reset();
}

// System Interface

void SkeletonBasedAnimationSystem::generateTasks( Core::TaskQueue* taskQueue,
                                                  const FrameInfo& frameInfo ) {
    for ( auto compEntry : m_components ) {
        // deal with AnimationComponents
        if ( auto animComp = dynamic_cast<SkeletonComponent*>( compEntry.second ) ) {
            if ( !Core::Math::areApproxEqual( m_time, frameInfo.m_animationTime ) ) {
                // here we update the skeleton w.r.t. the animation
                auto animFunc =
                    std::bind( &SkeletonComponent::update, animComp, frameInfo.m_animationTime );
                auto animTask = std::make_unique<Core::FunctionTask>(
                    animFunc, "AnimatorTask_" + animComp->getSkeleton()->getName() );
                taskQueue->registerTask( std::move( animTask ) );
            }
            else {
                // here we update the skeleton w.r.t. the manipulation
                auto animFunc = std::bind( &SkeletonComponent::updateDisplay, animComp );
                auto animTask = std::make_unique<Core::FunctionTask>(
                    animFunc, "AnimatorTask_" + animComp->getSkeleton()->getName() );
                taskQueue->registerTask( std::move( animTask ) );
            }
        }
        // deal with SkinningComponents
        else if ( auto skinComp = dynamic_cast<SkinningComponent*>( compEntry.second ) ) {
            auto skinFunc = std::bind( &SkinningComponent::skin, skinComp );
            auto skinTask = std::make_unique<Core::FunctionTask>(
                skinFunc, "SkinnerTask_" + skinComp->getMeshName() );
            auto endFunc = std::bind( &SkinningComponent::endSkinning, skinComp );
            auto endTask = std::make_unique<Core::FunctionTask>(
                endFunc, "SkinnerEndTask_" + skinComp->getMeshName() );

            auto skinTaskId = taskQueue->registerTask( std::move( skinTask ) );
            auto endTaskId  = taskQueue->registerTask( std::move( endTask ) );
            taskQueue->addPendingDependency( "AnimatorTask_" + skinComp->getSkeletonName(),
                                             skinTaskId );
            taskQueue->addDependency( skinTaskId, endTaskId );
        }
    }

    m_time = frameInfo.m_animationTime;
}

void SkeletonBasedAnimationSystem::handleAssetLoading( Entity* entity,
                                                       const Core::Asset::FileData* fileData ) {
    auto skelData = fileData->getHandleData();
    auto animData = fileData->getAnimationData();

    // deal with AnimationComponents
    Scalar startTime = std::numeric_limits<Scalar>::max();
    Scalar endTime   = 0;
    for ( const auto& skel : skelData ) {
        auto component = new SkeletonComponent( "AC_" + skel->getName(), entity );
        component->handleSkeletonLoading( skel );
        component->handleAnimationLoading( animData );
        auto [s, e] = component->getAnimationTimeInterval();
        startTime   = std::min( startTime, s );
        endTime     = std::max( endTime, e );
        component->setXray( m_xrayOn );
        registerComponent( entity, component );
    }
    // configure the time on the Engine
    auto engine = RadiumEngine::getInstance();
    engine->setStartTime( startTime );
    engine->setEndTime( endTime );

    // deal with SkinningComponents
    auto geomData = fileData->getGeometryData();
    if ( geomData.size() > 0 && skelData.size() > 0 ) {
        for ( const auto& geom : geomData ) {
            // look for a skeleton skinning this mesh
            // warning: there should be at most one such skeleton!
            auto it = std::find_if( skelData.begin(), skelData.end(), [&geom]( const auto& skel ) {
                return std::find_if( skel->getBindMeshes().begin(),
                                     skel->getBindMeshes().end(),
                                     [&geom]( const auto& meshName ) {
                                         return meshName == geom->getName();
                                     } ) != skel->getBindMeshes().end();
            } );
            if ( it != skelData.end() ) {
                const auto& skel             = *it;
                SkinningComponent* component = new SkinningComponent(
                    "SkC_" + geom->getName(), SkinningComponent::LBS, entity );
                component->handleSkinDataLoading( skel, geom->getName() );
                registerComponent( entity, component );
            }
        }
    }
}

// Skeleton display

void SkeletonBasedAnimationSystem::setXray( bool on ) {
    m_xrayOn = on;
    for ( const auto& comp : m_components ) {
        if ( auto animComp = dynamic_cast<SkeletonComponent*>( comp.second ) ) {
            animComp->setXray( on );
        }
    }
}

bool SkeletonBasedAnimationSystem::isXrayOn() {
    return m_xrayOn;
}

void SkeletonBasedAnimationSystem::toggleSkeleton( const bool status ) {
    for ( const auto& comp : m_components ) {
        if ( auto animComp = dynamic_cast<SkeletonComponent*>( comp.second ) ) {
            animComp->toggleSkeleton( status );
        }
    }
}

} // namespace Scene
} // namespace Engine
} // namespace Ra
