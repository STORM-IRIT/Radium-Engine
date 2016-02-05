#include <AnimationSystem.hpp>

#include <string>
#include <iostream>

#include <Core/Tasks/TaskQueue.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Entity/FrameInfo.hpp>
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
    }

    void AnimationSystem::generateTasks(Ra::Core::TaskQueue* taskQueue, const Ra::Engine::FrameInfo& frameInfo)
    {
        const bool playFrame = m_isPlaying || m_oneStep;

        Scalar currentDelta = playFrame ? frameInfo.m_dt : 0;

        for (auto compEntry : this->m_components)
        {
            AnimationComponent* component = std::static_pointer_cast<AnimationComponent>(compEntry.second).get();
            AnimatorTask* task = new AnimatorTask(component, currentDelta);
            taskQueue->registerTask( task );
        }

        m_oneStep = false;
    }

    Ra::Engine::Component* AnimationSystem::addComponentToEntityInternal(Ra::Engine::Entity* entity, uint id)
    {
        std::string componentName = "AnimationComponent_" + entity->getName() + std::to_string(id);
        AnimationComponent* component = new AnimationComponent(componentName);

        return component;
    }

    void AnimationSystem::reset()
    {
        for (auto compEntry : this->m_components)
        {
            AnimationComponent* component = std::static_pointer_cast<AnimationComponent>(compEntry.second).get();
            {
                component->reset();
            }
        }
        m_oneStep = true;
    }

    void AnimationSystem::step()
    {
        m_oneStep = true;
    }

    void AnimationSystem::setPlaying(bool isPlaying)
    {
        m_isPlaying = isPlaying;
    }

    void AnimationSystem::callbackOnComponentCreation(const Ra::Engine::Component *component)
    {

        // FIXME

        //std::cout << "Mesh component received by the Animation system" << std::endl;

        //FancyMeshPlugin::FancyMeshComponent* meshComponent = (FancyMeshPlugin::FancyMeshComponent*) component;

        //std::string fancy_name = meshComponent->getContentName();

        AnimationComponent* animationComponent = nullptr;
        for( auto& comp : m_components ) {
            animationComponent = static_cast<AnimationComponent*>( comp.second.get() );
            std::string anim_name = animationComponent->getContentName();
            //if( fancy_name == anim_name ) {
                break;
            //}
        }
        //if( animationComponent != nullptr ) animationComponent->setMeshComponent(meshComponent);


        //animationComponent->setMeshComponent(meshComponent);
        /*
        AnimationLoader::AnimationData componentData = AnimationLoader::loadFile(meshComponent->getLoadingInfo().filename, meshComponent->getLoadingInfo());
        if (componentData.hasLoaded)
        {
            AnimationComponent* animationComponent = static_cast<AnimationComponent*>(addComponentToEntity(meshComponent->getEntity()));
            animationComponent->setMeshComponent(meshComponent);
            animationComponent->handleLoading(componentData);

            callOnComponentCreationDependencies(animationComponent);
        }
        */
    }

    void AnimationSystem::handleAssetLoading( Ra::Engine::Entity* entity, const Ra::Asset::FileData* fileData ) {
        auto geomData = fileData->getGeometryData();
        auto skelData = fileData->getHandleData();
        auto animData = fileData->getAnimationData();

        // FIXME(Charly): One component of a given type by entity ?
        for ( const auto& skel : skelData )
        {
            uint geomID = uint(-1);
            for( uint i = 0; i < geomData.size(); ++i ) {
                if( skel->getName() == geomData[i]->getName() ) {
                    geomID = i;
                }
            }


            // FIXME(Charly): Certainly not the best way to do this
            AnimationComponent* component = static_cast<AnimationComponent*>(addComponentToEntity(entity));
            component->handleSkeletonLoading( skel, ( geomID == uint( -1 ) ) ? std::map< uint, uint >() : geomData[geomID]->getDuplicateTable() );
            component->handleAnimationLoading( animData );

            //ImplicitPlugin::ImplicitComponent* implicitComponent = static_cast<ImplicitPlugin::ImplicitComponent*>(addComponentToEntity(entity));
            //implicitComponent->setAnimationComponent(component);


        }
    }
}
