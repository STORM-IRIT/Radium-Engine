#include <simplifier.hpp>

#include <Core/Time/Timer.hpp>
#include <Core/Tasks/Task.hpp>
#include <Core/Tasks/TaskQueue.hpp>
#include <Core/Mesh/MeshPrimitives.hpp>
#include <Core/Mesh/Wrapper/TopologicalMeshConvert.hpp>
#include <Core/Utils/File/OBJFileManager.hpp>

#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Assets/FileData.hpp>
#include <Engine/Assets/HandleData.hpp>

#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>

#include <Engine/Managers/ComponentMessenger/ComponentMessenger.hpp>

#include <Core/Mesh/TriangleMesh.hpp>

SimplifierComponent::SimplifierComponent(const std::string& name) :
    Component(name)
{
    m_meshContactElement = new Ra::Engine::MeshContactElement();
}

/// This function is called when the component is properly
/// setup, i.e. it has an entity.
void  SimplifierComponent::initialize() {
}


void SimplifierComponent::handleMeshLoading( const Ra::Asset::GeometryData* data )
{
    m_contentsName = data->getName();
}

Ra::Engine::MeshContactElement* SimplifierComponent::getMeshContactElement()
{
    return m_meshContactElement;
}

void SimplifierComponent::exportMesh(std::string filename)
{
    Ra::Core::OBJFileManager obj;
    Ra::Core::TriangleMesh mesh;
    Ra::Core::MeshConverter::convert(*(m_meshContactElement->getProgressiveMeshLOD()->getProgressiveMesh()->getTopologicalMesh()), mesh);
    //Ra::Core::convertPM(*(m_meshContactElement->getProgressiveMeshLOD()->getProgressiveMesh()->getDcel()), mesh);
    bool result = obj.save( filename, mesh );
    if (result)
    {
        LOG(logINFO)<<"Mesh from "<<m_contentsName<<" successfully exported to "<<filename;
    }
    else
    {
        LOG(logERROR)<<"Mesh from "<<m_contentsName<<" failed to export";
    }
}

void SimplifierComponent::setupProgressiveMesh()
{
    bool hasMesh = Ra::Engine::ComponentMessenger::getInstance()->canGet<Ra::Core::TriangleMesh>(getEntity(), m_contentsName);
    if (hasMesh)
    {
        m_meshContactElement->addMesh(
                Ra::Engine::ComponentMessenger::getInstance()->ComponentMessenger::rwCallback<Ra::Core::Vector3Array>( getEntity(), m_contentsName+"v" ),
                Ra::Engine::ComponentMessenger::getInstance()->ComponentMessenger::rwCallback<Ra::Core::Vector3Array>( getEntity(), m_contentsName+"n" ),
                Ra::Engine::ComponentMessenger::getInstance()->ComponentMessenger::rwCallback<TriangleArray>( getEntity(), m_contentsName+"t" )
                );
    }
}

SimplifierSystem::SimplifierSystem()
{
    m_meshContactManager = new Ra::Engine::MeshContactManager();
}

std::vector<SimplifierComponent*> SimplifierSystem::getSimplifierComponents()
{
    return m_simplifierComponents;
}

void SimplifierSystem::handleAssetLoading(Ra::Engine::Entity *entity, const Ra::Asset::FileData *fileData)
{
    auto geomData = fileData->getGeometryData();

    uint id = 0;

    for ( const auto& data : geomData )
    {
        std::string componentName = "SMC_" + entity->getName() + std::to_string( id++ );
        SimplifierComponent * comp = new SimplifierComponent( componentName);
        comp->getMeshContactElement()->setIndex(m_components.size());
        entity->addComponent(comp);
        comp->handleMeshLoading(data);
        registerComponent(entity, comp);

        comp->setupProgressiveMesh();
        m_simplifierComponents.push_back(comp);
        m_meshContactManager->addMesh(comp->getMeshContactElement());
    }
}

void SimplifierSystem::generateTasks(Ra::Core::TaskQueue *q, const Ra::Engine::FrameInfo &info) {
   // We check that our component is here.
   CORE_ASSERT(m_components.size() == 1, "System incorrectly initialized");
}

Ra::Engine::MeshContactManager* SimplifierSystem::getMeshContactManager()
{
    return m_meshContactManager;
}
