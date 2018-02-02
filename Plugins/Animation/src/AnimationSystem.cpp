#include <AnimationSystem.hpp>

#include <string>
#include <iostream>

#include <Core/Tasks/TaskQueue.hpp>
#include <Core/Tasks/Task.hpp>
#include <Core/File/FileData.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/FrameInfo.hpp>

#include <AnimationComponent.hpp>
#include <Drawing/SkeletonBoneDrawable.hpp>

namespace AnimationPlugin
{
    AnimationSystem::AnimationSystem()
    {
        m_isPlaying = false;
        m_oneStep = false;
        m_xrayOn = false;
    }

    void AnimationSystem::generateTasks(Ra::Core::TaskQueue* taskQueue, const Ra::Engine::FrameInfo& frameInfo)
    {
        const bool playFrame = m_isPlaying || m_oneStep;

        Scalar currentDelta = playFrame ? frameInfo.m_dt : 0;

        for (auto compEntry : this->m_components)
        {
            AnimationComponent* component = static_cast<AnimationComponent*>(compEntry.second);
            Ra::Core::FunctionTask* task = new Ra::Core::FunctionTask(
                    std::bind(&AnimationComponent::update, component, currentDelta),
                    "AnimatorTask");
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
           static_cast<AnimationComponent*>(comp.second)->setXray(on);
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


    void AnimationSystem::toggleSkeleton( const bool status ) {
        for (const auto& comp : m_components)
        {
            static_cast<AnimationComponent*>(comp.second)->toggleSkeleton( status );
        }
    }

    void AnimationSystem::setAnimation( const uint i ) {
        for (const auto& comp : m_components)
        {
            static_cast<AnimationComponent*>(comp.second)->setAnimation( i );
        }
    }

    void AnimationSystem::toggleAnimationTimeStep( const bool status ) {
        for (const auto& comp : m_components)
        {
            static_cast<AnimationComponent*>(comp.second)->toggleAnimationTimeStep( status );
        }
    }

    void AnimationSystem::setAnimationSpeed( const Scalar value ) {
        for (const auto& comp : m_components)
        {
            static_cast<AnimationComponent*>(comp.second)->setSpeed( value );
        }
    }

    void AnimationSystem::toggleSlowMotion( const bool status ) {
        for (const auto& comp : m_components)
        {
            static_cast<AnimationComponent*>(comp.second)->toggleSlowMotion( status );
        }
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
            std::vector<Ra::Core::Index> dupliTable;
            uint nbMeshVertices = 0;
            if (geomID != uint(-1))
            {
                dupliTable     = geomData[geomID]->getDuplicateTable();
                nbMeshVertices = geomData[geomID]->getVerticesSize();
            }
            component->handleSkeletonLoading( skel, dupliTable, nbMeshVertices );
            component->handleAnimationLoading( animData );

            component->setXray( m_xrayOn );
            registerComponent( entity, component );
        }
    }

    Scalar AnimationSystem::getTime(const Ra::Engine::ItemEntry& entry) const
    {
        if (entry.isValid())
        {
            // If entry is an existing animation component, we return this one's time
            // if not, look for other components in this entity to see if some are animation
            std::vector<const AnimationComponent*> comps;
            for (const auto& ec : m_components)
            {
                if (ec.first == entry.m_entity)
                {
                    const AnimationComponent* c = static_cast< AnimationComponent*>(ec.second);
                    // Entry match, return that one
                    if (ec.second == c)
                    {
                        return c->getTime();
                    }
                    comps.push_back(c);
                }
            }
            // If comps is not empty, it means that we have a component in current entity
            // We just pick the first one
            if (!comps.empty())
            {
                return comps[0]->getTime();
            }
        }
        return 0.f;
    }

}
