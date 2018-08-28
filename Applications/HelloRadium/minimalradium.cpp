
#include <minimalradium.hpp>

#include <Core/Mesh/MeshPrimitives.hpp>
#include <Core/Tasks/Task.hpp>
#include <Core/Tasks/TaskQueue.hpp>
#include <Core/Time/Timer.hpp>

#include <Engine/Renderer/Mesh/Mesh.hpp>

#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>

/* This file contains a minimal radium/qt application which shows the
classic "Spinning Cube" demo. */

MinimalComponent::MinimalComponent( Ra::Engine::Entity* entity ) :
    Ra::Engine::Component( "Minimal Component", entity ) {}

void MinimalComponent::initialize() {
    // Create a cube mesh render object.
    std::shared_ptr<Ra::Engine::Mesh> display( new Ra::Engine::Mesh( "Cube" ) );
    display->loadGeometry( Ra::Core::MeshUtils::makeSharpBox( {0.1f, 0.1f, 0.1f} ) );
    auto renderObject = Ra::Engine::RenderObject::createRenderObject(
        "CubeRO", this, Ra::Engine::RenderObjectType::Fancy, display );
    addRenderObject( renderObject );
}

void MinimalComponent::spin() {
    Ra::Core::AngleAxis aa( 0.01f, Ra::Core::Vector3::UnitY() );
    Ra::Core::Transform rot( aa );

    auto ro = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject(
        m_renderObjects[0] );
    Ra::Core::Transform t = ro->getLocalTransform();
    ro->setLocalTransform( rot * t );
}

void MinimalSystem::generateTasks( Ra::Core::TaskQueue* q, const Ra::Engine::FrameInfo& info ) {
    // We check that our component is here.
    CORE_ASSERT( m_components.size() == 1, "System incorrectly initialized" );
    MinimalComponent* c = static_cast<MinimalComponent*>( m_components[0].second );

    // Create a new task which wil call c->spin() when executed.
    q->registerTask(
        new Ra::Core::FunctionTask( std::bind( &MinimalComponent::spin, c ), "spin" ) );
}
