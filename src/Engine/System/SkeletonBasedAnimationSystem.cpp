#include <SkeletonBasedAnimationSystem.hpp>

#include <iostream>
#include <string>

#include <Core/Asset/FileData.hpp>
#include <Core/Tasks/Task.hpp>
#include <Core/Tasks/TaskQueue.hpp>

#include <Engine/FrameInfo.hpp>
#include <Engine/RadiumEngine.hpp>

#include <Drawing/SkeletonBoneDrawable.hpp>
#include <SkeletonComponent.hpp>
#include <SkinningComponent.hpp>

namespace SkeletonBasedAnimationPlugin {

SkeletonBasedAnimationSystem::SkeletonBasedAnimationSystem() :
    m_animFrame( 0 ),
    m_isPlaying( false ),
    m_oneStep( false ),
    m_xrayOn( false ) {}

void SkeletonBasedAnimationSystem::generateTasks( Ra::Core::TaskQueue* taskQueue,
                                                  const Ra::Engine::FrameInfo& frameInfo ) {
    const bool playFrame = m_isPlaying || m_oneStep;

    if ( playFrame ) { ++m_animFrame; }

    // deal with AnimationComponents
    Scalar currentDelta = playFrame ? frameInfo.m_dt : 0;
    for ( auto compEntry : m_components )
    {
        if ( compEntry.second->getName().compare( 0, 3, "AC_" ) == 0 )
        {
            auto animComp = static_cast<SkeletonComponent*>( compEntry.second );
            auto animFunc = std::bind( &SkeletonComponent::update, animComp, currentDelta );
            auto animTask = new Ra::Core::FunctionTask(
                animFunc, "AnimatorTask_" + animComp->getSkeleton().getName() );
            taskQueue->registerTask( animTask );
        }
        else if ( compEntry.second->getName().compare( 0, 4, "SkC_" ) == 0 )
        {
            auto skinComp = static_cast<SkinningComponent*>( compEntry.second );
            auto skinFunc = std::bind( &SkinningComponent::skin, skinComp );
            auto skinTask =
                new Ra::Core::FunctionTask( skinFunc, "SkinnerTask_" + skinComp->getMeshName() );
            auto endFunc = std::bind( &SkinningComponent::endSkinning, skinComp );
            auto endTask =
                new Ra::Core::FunctionTask( endFunc, "SkinnerEndTask_" + skinComp->getMeshName() );

            Ra::Core::TaskQueue::TaskId skinTaskId = taskQueue->registerTask( skinTask );
            Ra::Core::TaskQueue::TaskId endTaskId  = taskQueue->registerTask( endTask );
            taskQueue->addPendingDependency( "AnimatorTask_" + skinComp->getSkeletonName(),
                                             skinTaskId );
            taskQueue->addDependency( skinTaskId, endTaskId );
        }
    }

    CoupledTimedSystem::generateTasks( taskQueue, frameInfo );

    m_oneStep = false;
}

void SkeletonBasedAnimationSystem::play( bool isPlaying ) {
    m_isPlaying = isPlaying;
    CoupledTimedSystem::play( isPlaying );
}

void SkeletonBasedAnimationSystem::step() {
    m_oneStep = true;
    CoupledTimedSystem::step();
}

void SkeletonBasedAnimationSystem::reset() {
    m_animFrame = 0;

    // deal with AnimationComponents
    for ( auto compEntry : m_components )
    {
        if ( compEntry.second->getName().compare( 0, 3, "AC_" ) == 0 )
        {
            auto component = static_cast<SkeletonComponent*>( compEntry.second );
            { component->reset(); }
        }
    }
    CoupledTimedSystem::reset();
}

bool SkeletonBasedAnimationSystem::isXrayOn() {
    return m_xrayOn;
}

void SkeletonBasedAnimationSystem::setXray( bool on ) {
    m_xrayOn = on;
    for ( const auto& comp : m_components )
    {
        if ( comp.second->getName().compare( 0, 3, "AC_" ) == 0 )
        { static_cast<SkeletonComponent*>( comp.second )->setXray( on ); } }
}

void SkeletonBasedAnimationSystem::toggleSkeleton( const bool status ) {
    for ( const auto& comp : m_components )
    {
        if ( comp.second->getName().compare( 0, 3, "AC_" ) == 0 )
        { static_cast<SkeletonComponent*>( comp.second )->toggleSkeleton( status ); } }
}

void SkeletonBasedAnimationSystem::setAnimation( const uint i ) {
    for ( const auto& comp : m_components )
    {
        if ( comp.second->getName().compare( 0, 3, "AC_" ) == 0 )
        { static_cast<SkeletonComponent*>( comp.second )->setAnimation( i ); } }
}

void SkeletonBasedAnimationSystem::toggleAnimationTimeStep( const bool status ) {
    for ( const auto& comp : m_components )
    {
        if ( comp.second->getName().compare( 0, 3, "AC_" ) == 0 )
        { static_cast<SkeletonComponent*>( comp.second )->toggleAnimationTimeStep( status ); } }
}

void SkeletonBasedAnimationSystem::setAnimationSpeed( const Scalar value ) {
    for ( const auto& comp : m_components )
    {
        if ( comp.second->getName().compare( 0, 3, "AC_" ) == 0 )
        { static_cast<SkeletonComponent*>( comp.second )->setSpeed( value ); } }
}

void SkeletonBasedAnimationSystem::toggleSlowMotion( const bool status ) {
    for ( const auto& comp : m_components )
    {
        if ( comp.second->getName().compare( 0, 3, "AC_" ) == 0 )
        { static_cast<SkeletonComponent*>( comp.second )->toggleSlowMotion( status ); } }
}

void SkeletonBasedAnimationSystem::handleAssetLoading( Ra::Engine::Entity* entity,
                                                       const Ra::Core::Asset::FileData* fileData ) {
    auto skelData = fileData->getHandleData();
    auto animData = fileData->getAnimationData();

    // deal with AnimationComponents
    for ( const auto& skel : skelData )
    {
        auto component = new SkeletonComponent( "AC_" + skel->getName(), entity );
        component->handleSkeletonLoading( skel );
        component->handleAnimationLoading( animData );

        component->setXray( m_xrayOn );
        registerComponent( entity, component );
    }

    // deal with SkinningComponents
    auto geomData = fileData->getGeometryData();
    if ( geomData.size() > 0 && skelData.size() > 0 )
    {
        for ( const auto& geom : geomData )
        {
            // look for a skeleton skinning this mesh
            // warning: there should be at most one such skeleton!
            auto it = std::find_if( skelData.begin(), skelData.end(), [&geom]( const auto& skel ) {
                return std::find_if( skel->getBindMeshes().begin(),
                                     skel->getBindMeshes().end(),
                                     [&geom]( const auto& meshName ) {
                                         return meshName == geom->getName();
                                     } ) != skel->getBindMeshes().end();
            } );
            if ( it != skelData.end() )
            {
                const auto& skel             = *it;
                SkinningComponent* component = new SkinningComponent(
                    "SkC_" + geom->getName(), SkinningComponent::LBS, entity );
                component->handleSkinDataLoading( skel, geom->getName(), geom->getFrame() );
                registerComponent( entity, component );
            }
        }
    }

    CoupledTimedSystem::handleAssetLoading( entity, fileData );
}

Scalar SkeletonBasedAnimationSystem::getTime( const Ra::Engine::ItemEntry& entry ) const {
    if ( entry.isValid() )
    {
        // If entry is an existing animation component, we return this one's time
        // if not, look for other components in this entity to see if some are animation
        std::vector<const SkeletonComponent*> comps;
        for ( const auto& ec : m_components )
        {
            if ( ec.first == entry.m_entity && ec.second->getName().compare( 0, 3, "AC_" ) == 0 )
            {
                const auto c = static_cast<SkeletonComponent*>( ec.second );
                // Entry match, return that one
                if ( ec.second == entry.m_component ) { return c->getTime(); }
                comps.push_back( c );
            }
        }
        // If comps is not empty, it means that we have a component in current entity
        // We just pick the first one
        if ( !comps.empty() ) { return comps[0]->getTime(); }
    }
    return 0.f;
}

uint SkeletonBasedAnimationSystem::getMaxFrame() const {
    uint m = 0;
    for ( const auto& comp : m_components )
    {
        if ( comp.second->getName().compare( 0, 3, "AC_" ) == 0 )
        { m = std::max( m, static_cast<SkeletonComponent*>( comp.second )->getMaxFrame() ); } }
    return m;
}

void SkeletonBasedAnimationSystem::cacheFrame( const std::string& dir, uint frameId ) const {
    // deal with AnimationComponents
    for ( const auto& comp : m_components )
    {
        if ( comp.second->getName().compare( 0, 3, "AC_" ) == 0 )
        { static_cast<SkeletonComponent*>( comp.second )->cacheFrame( dir, frameId ); } }

    CoupledTimedSystem::cacheFrame( dir, frameId );
}

bool SkeletonBasedAnimationSystem::restoreFrame( const std::string& dir, uint frameId ) {
    static bool restoringCurrent = false;
    if ( !restoringCurrent )
    {
        // first save current, in case restoration fails.
        cacheFrame( dir, m_animFrame );
    }
    bool success = true;
    // deal with AnimationComponents
    for ( const auto& comp : m_components )
    {
        if ( comp.second->getName().compare( 0, 3, "AC_" ) == 0 )
        {
            success &= static_cast<SkeletonComponent*>( comp.second )->restoreFrame( dir, frameId );
        }
    }
    // if fail, restore current frame
    if ( !success && !restoringCurrent )
    {
        restoringCurrent = true;
        restoreFrame( dir, m_animFrame );
        restoringCurrent = false;
        return false;
    }

    success &= CoupledTimedSystem::restoreFrame( dir, frameId );
    // if fail, restore current frame
    if ( !success && !restoringCurrent )
    {
        restoringCurrent = true;
        restoreFrame( dir, m_animFrame );
        restoringCurrent = false;
        return false;
    }

    if ( success ) { m_animFrame = frameId; }
    return success;
}

void SkeletonBasedAnimationSystem::showWeights( bool on ) {
    for ( auto& compEntry : m_components )
    {
        if ( compEntry.second->getName().compare( 0, 4, "SkC_" ) == 0 )
        { static_cast<SkinningComponent*>( compEntry.second )->showWeights( on ); } }
}

void SkeletonBasedAnimationSystem::showWeightsType( int type ) {
    for ( auto& compEntry : m_components )
    {
        if ( compEntry.second->getName().compare( 0, 4, "SkC_" ) == 0 )
        { static_cast<SkinningComponent*>( compEntry.second )->showWeightsType( type ); } }
}

} // namespace SkeletonBasedAnimationPlugin
