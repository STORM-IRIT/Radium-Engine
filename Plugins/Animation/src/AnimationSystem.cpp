#include <AnimationSystem.hpp>

#include <iostream>
#include <string>

#include <Core/File/FileData.hpp>
#include <Core/Tasks/Task.hpp>
#include <Core/Tasks/TaskQueue.hpp>

#include <Engine/FrameInfo.hpp>
#include <Engine/RadiumEngine.hpp>

#include <AnimationComponent.hpp>
#include <Drawing/SkeletonBoneDrawable.hpp>

namespace AnimationPlugin {

AnimationSystem::AnimationSystem() {
    m_isPlaying = false;
    m_oneStep = false;
    m_xrayOn = false;
}

void AnimationSystem::generateTasks( Ra::Core::TaskQueue* taskQueue,
                                     const Ra::Engine::FrameInfo& frameInfo ) {
    const bool playFrame = m_isPlaying || m_oneStep;

    if ( playFrame )
    {
        ++m_animFrame;
    }

    // deal with AnimationComponents
    Scalar currentDelta = playFrame ? frameInfo.m_dt : 0;
    for ( auto compEntry : this->m_components )
    {
        auto animComp = static_cast<AnimationComponent*>( compEntry.second );
        auto animFunc = std::bind( &AnimationComponent::update, animComp, currentDelta );
        auto animTask = new Ra::Core::FunctionTask( animFunc, "AnimatorTask" );
        taskQueue->registerTask( animTask );
    }

    // deal with coupled systems
    for ( auto s : this->m_systems )
    {
        s->generateTasks( taskQueue, frameInfo );
    }

    m_oneStep = false;
}

void AnimationSystem::play( bool isPlaying ) {
    m_isPlaying = isPlaying;

    // deal with coupled systems
    for ( auto s : this->m_systems )
    {
        s->play( isPlaying );
    }
}

void AnimationSystem::step() {
    m_oneStep = true;

    // deal with coupled systems
    for ( auto s : this->m_systems )
    {
        s->step();
    }
}

void AnimationSystem::reset() {
    m_animFrame = 0;

    // deal with AnimationComponents
    for ( auto compEntry : this->m_components )
    {
        AnimationComponent* component = static_cast<AnimationComponent*>( compEntry.second );
        { component->reset(); }
    }

    // deal with coupled systems
    for ( auto s : this->m_systems )
    {
        s->reset();
    }
}

bool AnimationSystem::isXrayOn() {
    return m_xrayOn;
}

void AnimationSystem::setXray( bool on ) {
    m_xrayOn = on;
    for ( const auto& comp : m_components )
    {
        static_cast<AnimationComponent*>( comp.second )->setXray( on );
    }
}

void AnimationSystem::toggleSkeleton( const bool status ) {
    for ( const auto& comp : m_components )
    {
        static_cast<AnimationComponent*>( comp.second )->toggleSkeleton( status );
    }
}

void AnimationSystem::setAnimation( const uint i ) {
    for ( const auto& comp : m_components )
    {
        static_cast<AnimationComponent*>( comp.second )->setAnimation( i );
    }
}

void AnimationSystem::toggleAnimationTimeStep( const bool status ) {
    for ( const auto& comp : m_components )
    {
        static_cast<AnimationComponent*>( comp.second )->toggleAnimationTimeStep( status );
    }
}

void AnimationSystem::setAnimationSpeed( const Scalar value ) {
    for ( const auto& comp : m_components )
    {
        static_cast<AnimationComponent*>( comp.second )->setSpeed( value );
    }
}

void AnimationSystem::toggleSlowMotion( const bool status ) {
    for ( const auto& comp : m_components )
    {
        static_cast<AnimationComponent*>( comp.second )->toggleSlowMotion( status );
    }
}

void AnimationSystem::handleAssetLoading( Ra::Engine::Entity* entity,
                                          const Ra::Asset::FileData* fileData ) {
    auto geomData = fileData->getGeometryData();
    auto skelData = fileData->getHandleData();
    auto animData = fileData->getAnimationData();

    // deal with AnimationComponents
    for ( const auto& skel : skelData )
    {
        uint geomID = uint( -1 );
        for ( uint i = 0; i < geomData.size(); ++i )
        {
            if ( skel->getName() == geomData[i]->getName() )
            {
                geomID = i;
            }
        }

        // FIXME(Charly): Certainly not the best way to do this
        auto component = new AnimationComponent( "AC_" + skel->getName(), entity );
        std::vector<Ra::Core::Index> dupliTable;
        uint nbMeshVertices = 0;
        if ( geomID != uint( -1 ) )
        {
            dupliTable = geomData[geomID]->getDuplicateTable();
            nbMeshVertices = geomData[geomID]->getVerticesSize();
        }
        component->handleSkeletonLoading( skel, dupliTable, nbMeshVertices );
        component->handleAnimationLoading( animData );

        component->setXray( m_xrayOn );
        registerComponent( entity, component );
    }

    // deal with coupled systems
    for ( auto s : this->m_systems )
    {
        s->handleAssetLoading( entity, fileData );
    }
}

Scalar AnimationSystem::getTime( const Ra::Engine::ItemEntry& entry ) const {
    if ( entry.isValid() )
    {
        // If entry is an existing animation component, we return this one's time
        // if not, look for other components in this entity to see if some are animation
        std::vector<const AnimationComponent*> comps;
        for ( const auto& ec : m_components )
        {
            if ( ec.first == entry.m_entity )
            {
                const auto c = static_cast<AnimationComponent*>( ec.second );
                // Entry match, return that one
                if ( ec.second == c )
                {
                    return c->getTime();
                }
                comps.push_back( c );
            }
        }
        // If comps is not empty, it means that we have a component in current entity
        // We just pick the first one
        if ( !comps.empty() )
        {
            return comps[0]->getTime();
        }
    }
    return 0.f;
}

void AnimationSystem::cacheFrame() const {
    // deal with AnimationComponents
    for ( const auto& comp : m_components )
    {
        static_cast<AnimationComponent*>( comp.second )->cacheFrame( m_animFrame );
    }
    // deal with coupled systems
    for ( const auto &s : this->m_systems )
    {
        s->cacheFrame( m_animFrame );
    }
}

bool AnimationSystem::restoreFrame( uint frame ) {
    static bool restoringCurrent = false;
    if (!restoringCurrent)
    {
        // first save current, in case restoration fails.
        cacheFrame();
    }
    bool success = true;
    // deal with AnimationComponents
    for ( const auto& comp : m_components )
    {
        success &= static_cast<AnimationComponent*>( comp.second )->restoreFrame( frame );
    }
    // if fail, restore current frame
    if ( !success && !restoringCurrent )
    {
        restoringCurrent = true;
        restoreFrame( m_animFrame );
        restoringCurrent = false;
        return false;
    }
    CORE_ASSERT( success, "Error while trying to restore current frame" );
    // deal with coupled systems
    for ( const auto &s : this->m_systems )
    {
        success &= s->restoreFrame( frame );
    }
    // if fail, restore current frame
    if ( !success && !restoringCurrent )
    {
        restoringCurrent = true;
        restoreFrame( m_animFrame );
        restoringCurrent = false;
        return false;
    }
    CORE_ASSERT( success, "Error while trying to restore current frame" );
    if ( success )
    {
        m_animFrame = frame;
    }
    return success;
}

} // namespace AnimationPlugin
