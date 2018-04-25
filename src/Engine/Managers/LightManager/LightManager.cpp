#include <Engine/Managers/LightManager/LightManager.hpp>
#include <Engine/Renderer/Light/DirLight.hpp>
#include <Engine/Renderer/Light/PointLight.hpp>
#include <Engine/Renderer/Light/SpotLight.hpp>

#include <Core/Asset/FileData.hpp>
#include <Core/Asset/GeometryData.hpp>
#include <Core/Utils/Task.hpp>
#include <Core/Utils/TaskQueue.hpp>

#include <Engine/Entity/Entity.hpp>
#include <Engine/FrameInfo.hpp>
#include <Engine/Managers/ComponentMessenger/ComponentMessenger.hpp>
#include <Engine/RadiumEngine.hpp>

namespace Ra {
namespace Engine {

LightManager::LightManager() : m_data( nullptr ) {}

LightManager::~LightManager() {}

//
// Renderer pre/post calls
//

size_t LightManager::count() const {
    return m_data->size();
}

//
// System
//

void LightManager::generateTasks( Core::Utils::TaskQueue* taskQueue, const Engine::FrameInfo& frameInfo ) {
    /*
    futur work for async rendering
    Ra::Core::Utils::FunctionTask* preprocess_task = new Ra::Core::Utils::FunctionTask(
        std::bind(&LightManager::preprocess, this),
        "PreProcessLight"
    );

    Ra::Core::Utils::TaskQueue::TaskId preprocess_id = taskQueue->registerTask( preprocess_task );
    Ra::Core::Utils::TaskQueue::TaskId lastprocess_id = preprocess_id;

    for (const auto& compEntry : m_components)
    {
        Light* comp = static_cast<Light*>( compEntry.second );

        if(!comp)
            continue;

        Ra::Core::Utils::FunctionTask* prerender_task = new Ra::Core::Utils::FunctionTask(
           [this,comp]() { this->prerender(*comp); }, "PreRenderLight"
        );

        Ra::Core::Utils::FunctionTask* postrender_task = new Ra::Core::Utils::FunctionTask(
           [this,comp]() { this->postrender(*comp); }, "PostRenderLight"
        );

        Ra::Core::Utils::TaskQueue::TaskId prerender_id = taskQueue->registerTask( prerender_task );
        Ra::Core::Utils::TaskQueue::TaskId postrender_id = taskQueue->registerTask( postrender_task );

        taskQueue->addDependency( preprocess_id, prerender_id);
        taskQueue->addDependency( prerender_id, postrender_id);

        lastprocess_id = postrender_id;
    }

    Ra::Core::Utils::FunctionTask* postprocess_task = new Ra::Core::Utils::FunctionTask(
        std::bind(&LightManager::postprocess, this),
        "PostProcess"
    );

    Ra::Core::Utils::TaskQueue::TaskId postprocess_id = taskQueue->registerTask( postprocess_task );

    taskQueue->addDependency( postprocess_id, lastprocess_id);
    */
}

void LightManager::handleAssetLoading( Entity* entity, const Core::Asset::FileData* filedata ) {
    std::vector<Core::Asset::LightData*> lightData = filedata->getLightData();
    uint id = 0;
    m_data->clear();
    for ( const auto& data : lightData )
    {
        std::string componentName = "LIGHT_" + entity->getName() + std::to_string( id++ );
        Light* comp = nullptr;

        switch ( data->getType() )
        {
        case Core::Asset::LightData::DIRECTIONAL_LIGHT:
        {
            auto thelight = new Engine::DirectionalLight( entity, data->getName() );
            thelight->setColor( data->m_color );
            thelight->setDirection( data->m_dirlight.direction );
            comp = thelight;
            break;
        }
        case Core::Asset::LightData::POINT_LIGHT:
        {
            auto thelight = new Engine::PointLight( entity, data->getName() );
            thelight->setColor( data->m_color );
            thelight->setPosition( data->m_pointlight.position );
            thelight->setAttenuation( data->m_pointlight.attenuation.constant,
                                      data->m_pointlight.attenuation.linear,
                                      data->m_pointlight.attenuation.quadratic );
            comp = thelight;
            break;
        }
        case Core::Asset::LightData::SPOT_LIGHT:
        {
            auto thelight = new Engine::SpotLight( entity, data->getName() );
            thelight->setColor( data->m_color );
            thelight->setPosition( data->m_spotlight.position );
            thelight->setDirection( data->m_spotlight.direction );
            thelight->setAttenuation( data->m_spotlight.attenuation.constant,
                                      data->m_spotlight.attenuation.linear,
                                      data->m_spotlight.attenuation.quadratic );
            thelight->setInnerAngleInRadians( data->m_spotlight.innerAngle );
            thelight->setOuterAngleInRadians( data->m_spotlight.outerAngle );
            comp = thelight;
            break;
        }
        case Core::Asset::LightData::AREA_LIGHT:
        {
            // No arealight for now (see pbrplugin)
            comp = nullptr;
            break;
        }
        default:
            comp = nullptr;
        }

        //! @comp should be allocated in LightStorage (well, not sure ...)

        if ( !comp )
            continue;

        registerComponent( entity, comp );
    }
    LOG( Core::Utils::logINFO ) << "LightManager : loaded " << count() << " lights.";
}

void LightManager::registerComponent( const Entity* entity, Component* component ) {
    System::registerComponent(entity, component);
    m_data->add(reinterpret_cast<Light *>(component));
}

void LightManager::unregisterComponent( const Entity* entity, Component* component ) {
    m_data->remove( reinterpret_cast<Light *>(component) );
    System::unregisterComponent(entity, component);

}

void LightManager::unregisterAllComponents( const Entity* entity ) {
    m_data->clear();
    System::unregisterAllComponents(entity);
}


  } // namespace Engine
} // namespace Ra
