#include <AnimationSystem.hpp>

#include <string>
#include <iostream>

#include <Core/Tasks/TaskQueue.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/FrameInfo.hpp>
#include <Engine/Assets/FileData.hpp>
#include <Engine/Assets/HandleData.hpp>

#include <AnimationComponent.hpp>
#include <AnimatorTask.hpp>
#include <Drawing/SkeletonBoneDrawable.hpp>


namespace AnimationPlugin
{
    AnimationSystem::AnimationSystem()
    {
        m_isPlaying = false;
        m_oneStep = false;
        m_xrayOn = true;
    }

    void AnimationSystem::generateTasks(Ra::Core::TaskQueue* taskQueue, const Ra::Engine::FrameInfo& frameInfo)
    {
        const bool playFrame = m_isPlaying || m_oneStep;

        Scalar currentDelta = playFrame ? frameInfo.m_dt : 0;

        for (auto compEntry : this->m_components)
        {
            AnimationComponent* component = static_cast<AnimationComponent*>(compEntry.second);
            AnimatorTask* task = new AnimatorTask(component, currentDelta);
            taskQueue->registerTask( task );
        }

        m_oneStep = false;
    }

    void AnimationSystem::reset()
    {
        for (auto compEntry : this->m_components)
        {
            AnimationComponent* component = static_cast<AnimationComponent*>(compEntry.second);
            {
                component->reset();
            }
        }
    }

    bool AnimationSystem::isXrayOn()
    {
       return m_xrayOn;
    }

    void AnimationSystem::setXray(bool on)
    {
       m_xrayOn = on;
       for (const auto& comp : m_components)
       {
           static_cast<AnimationComponent*>(comp.second)->toggleXray(on);
       }
    }

    void AnimationSystem::step()
    {
        m_oneStep = true;
    }

    void AnimationSystem::setPlaying( bool isPlaying )
    {
        m_isPlaying = isPlaying;
    }

    void AnimationSystem::handleAssetLoading( Ra::Engine::Entity* entity, const Ra::Asset::FileData* fileData ) {
        auto geomData = fileData->getGeometryData();
        auto skelData = fileData->getHandleData();
        auto animData = fileData->getAnimationData();

        for ( const auto& skel : skelData )
        {
            uint geomID = uint(-1);
            for( uint i = 0; i < geomData.size(); ++i ) {
                if( skel->getName() == geomData[i]->getName() ) {
                    geomID = i;
                }
            }

            // FIXME(Charly): Certainly not the best way to do this
            AnimationComponent* component = new AnimationComponent( "AC_" + skel->getName() );
            entity->addComponent( component );
            component->handleSkeletonLoading( skel, ( geomID == uint( -1 ) ) ? std::map< uint, uint >() : geomData[geomID]->getDuplicateTable() );
            component->handleAnimationLoading( animData );

            component->toggleXray( m_xrayOn );
            registerComponent( entity, component );
        }
    }
}
